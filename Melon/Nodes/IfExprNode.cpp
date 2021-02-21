#include "IfExprNode.h"

#include "StackNode.h"
#include "TypeNode.h"
#include "ConvertNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

IfExprNode::IfExprNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {
	
}

IfExprNode::~IfExprNode() {

}

TypeSymbol* IfExprNode::Type() const {
	TypeSymbol* type = nodes[0]->Type();

	for (UInt i = 1; i < nodes.Size(); i++) {
		if (type != nodes[i]->Type()) ErrorLog::Error(TypeError(TypeError::IfExprType, file));
	}

	return type;
}

CompiledNode IfExprNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	// Setup return value
	cn.size = Type()->Size();

	List<UInt> jumps;

	info.stack.PushExpr(cn.size, cn);
	const UInt frame = info.stack.frame; 

	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	Pointer<StackNode> sn = new StackNode(info.stack.top);
	sn->type = Type()->AbsoluteName();

	StackPtr stack = info.stack;

	// Compile conditions and segments
	for (UInt i = 0; i < conditions.Size(); i++) {
		info.stack = stack;
		info.stack.PopExpr(frame, cn);

		// Compile condition
		CompiledNode cond = conditions[i]->Compile(info);
		cn.AddInstructions(cond.instructions);

		stack = info.stack;

		Instruction condInst = Instruction(InstructionType::Eq, 1);
		condInst.arguments.Add(cond.argument);
		condInst.arguments.Add(Argument(0));
		cn.instructions.Add(condInst);

		UInt jump = cn.instructions.Size() - 1;

		// Compile value assignment
		cn.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->file).instructions);

		info.stack.PopExpr(frame, cn);

		// Add jump and label
		jumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));

		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		cn.instructions.Add(Instruction::Label(info.label++));
	}

	info.stack = stack;

	// Compile else
	cn.AddInstructions(CompileAssignment(sn, nodes.Last(), info, nodes.Last()->file).instructions);

	info.stack.PopExpr(frame, cn);

	// Add jumps to end
	for (UInt i : jumps) {
		cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	// End label
	cn.instructions.Add(Instruction::Label(info.label++));

	info.stack.Pop(Type()->Size());
	return cn;
}

void IfExprNode::IncludeScan(ParsingInfo& info) {
	for (NodePtr node : nodes) {
		node->IncludeScan(info);
	}

	for (NodePtr condition : conditions) {
		condition->IncludeScan(info);
	}
}

ScanResult IfExprNode::Scan(ScanInfoStack& info) {
	ScanResult result;
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	TypeSymbol* const t = Type();
	Pointer<TypeNode> type = t ? new TypeNode(t->AbsoluteName()) : nullptr;

	// Scan values
	for (const NodePtr& node : nodes) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;

		if (type) {
			ScanAssignment(type, node, info, node->file);
		}
	}

	// Scan conditions
	for (const NodePtr& node : conditions) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;
	}

	info->scopeInfo = scopeInfo;
	return result;
}

ScopeList IfExprNode::FindSideEffectScope(const bool assign) {
	ScopeList list = conditions[0]->GetSideEffectScope(assign);

	for (UInt i = 1; i < conditions.Size(); i++) {
		list = CombineSideEffects(list, conditions[i]->GetSideEffectScope(assign));
	}

	for (NodePtr& node : nodes) {
		list = CombineSideEffects(list, node->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr IfExprNode::Optimize(OptimizeInfo& info) {
	// Optimize nodes and conditions
	for (NodePtr& node : nodes) {
		if (NodePtr n = node->Optimize(info)) node = n;
	}

	for (NodePtr& cond : conditions) {
		if (NodePtr n = cond->Optimize(info)) cond = n;
	}

	// TODO: save type before this
	// Remove segments
	for (UInt i = 0; i < conditions.Size(); i++) {
		if (conditions[i]->IsImmediate()) {
			// Remove segment if condition is false
			if (conditions[i]->GetImmediate() == 0) {
				conditions.RemoveAt(i);
				nodes.RemoveAt(i);
				i--;
				info.optimized = true;
			}
			// Remove remaining segments if condition is true
			else {
				nodes.RemoveAt(i + 1, nodes.Size() - i - 1);
				conditions.RemoveAt(i, conditions.Size() - i);
				info.optimized = true;
				break;
			}
		}
	}

	// Replcae if expression with a value if there is only one segment
	if (nodes.Size() == 1) {
		if (nodes[0]->Type() != Type()) {
			Pointer<ConvertNode> cn = new ConvertNode(nodes[0]->scope, nodes[0]->file);
			cn->isExplicit = true;
			cn->node = nodes[0];
			cn->type = Type()->AbsoluteName();
			info.optimized = true;
			return cn;
		}
		else {
			info.optimized = true;
			return nodes[0];
		}
	}

	return nullptr;
}

StringBuilder IfExprNode::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs1 = String('\t').Repeat(indent);
	String tabs2 = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < nodes.Size(); i++) {
		if (i == 0) {
			sb += "if ";
		}
		else if (i < conditions.Size()) {
			sb += tabs1;
			sb += "elseif ";
		}
		else {
			sb += tabs1;
			sb += "else";
		}

		if (i < conditions.Size()) {
			sb += conditions[i]->ToMelon(indent);
			sb += " then\n";
		}
		else {
			sb += "\n";
		}

		sb += tabs2;
		sb += nodes[i]->ToMelon(indent + 1);
		sb += "\n";
	}

	sb += tabs1;
	sb += "end";

	return sb;
}