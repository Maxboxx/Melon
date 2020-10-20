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

IfExprNode::IfExprNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {
	
}

IfExprNode::~IfExprNode() {

}

ScopeList IfExprNode::Type() const {
	ScopeList type = nodes[0]->Type();

	for (UInt i = 1; i < nodes.Size(); i++) {
		if (type != nodes[i]->Type()) ErrorLog::Error(TypeError(TypeError::IfExprType, file));
	}

	return type;
}

CompiledNode IfExprNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	info.stack.Push(Symbol::Find(Type(), file).size);
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	Pointer<StackNode> sn = new StackNode(cn.argument.mem.offset);
	sn->type = Type();

	for (UInt i = 0; i < conditions.Size(); i++) {
		CompiledNode cond = conditions[i]->Compile(info);

		for (const OptimizerInstruction& inst : cond.instructions) {
			cn.instructions.Add(inst);
		}

		Instruction condInst = Instruction(InstructionType::Eq, 1);
		condInst.arguments.Add(cond.argument);
		condInst.arguments.Add(Argument(0));
		cn.instructions.Add(condInst);

		UInt jump = cn.instructions.Size() - 1;

		cn.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->file).instructions);

		jumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));

		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		cn.instructions.Add(Instruction::Label(info.label++));
	}

	cn.AddInstructions(CompileAssignment(sn, nodes.Last(), info, nodes.Last()->file).instructions);

	for (UInt i : jumps) {
		cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	cn.instructions.Add(Instruction::Label(info.label++));

	info.stack.Pop(Symbol::Find(Type(), file).size);

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

Set<ScanType> IfExprNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBranch();

	Pointer<TypeNode> type = new TypeNode(Type());

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}

		ScanAssignment(type, node, info, node->file);
	}

	for (const NodePtr& node : conditions) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	Symbol::Find(Type(), file);

	info.Get().scopeInfo = scopeInfo;
	return scanSet;
}

NodePtr IfExprNode::Optimize(OptimizeInfo& info) {
	for (NodePtr& node : nodes) {
		if (NodePtr n = node->Optimize(info)) node = n;
	}

	for (NodePtr& cond : conditions) {
		if (NodePtr n = cond->Optimize(info)) cond = n;
	}

	// TODO: save type before this
	for (UInt i = 0; i < conditions.Size(); i++) {
		if (conditions[i]->IsImmediate()) {
			if (conditions[i]->GetImmediate() == 0) {
				conditions.RemoveAt(i);
				nodes.RemoveAt(i);
				i--;
			}
			else {
				nodes.RemoveAt(i + 1, nodes.Size() - i - 1);
				conditions.RemoveAt(i, conditions.Size() - i);
				break;
			}
		}
	}

	if (nodes.Size() == 1) {
		if (nodes[0]->Type() != Type()) {
			Pointer<ConvertNode> cn = new ConvertNode(nodes[0]->scope, nodes[0]->file);
			cn->isExplicit = true;
			cn->node = nodes[0];
			cn->type = Type();
			return cn;
		}
		else {
			return nodes[0];
		}
	}

	return nullptr;
}

Mango IfExprNode::ToMango() const {
	Mango mango = Mango("ifexp", MangoType::List);

	for (UInt i = 0; i < nodes.Size(); i++) {
		Mango m = Mango(MangoType::Map);

		if (i == 0) m.SetLabel("if");
		else if (i < conditions.Size()) m.SetLabel("elseif");
		else m.SetLabel("else");

		if (i < conditions.Size()) {
			m.Add("condition", conditions[i]->ToMango());
		}

		m.Add("expr", nodes[i]->ToMango());

		mango.Add(m);
	}

	return mango;
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