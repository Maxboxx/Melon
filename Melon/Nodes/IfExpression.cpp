#include "IfExpression.h"

#include "StackExpression.h"
#include "TypeExpression.h"
#include "TypeConversion.h"
#include "Condition.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

IfExpression::IfExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {
	
}

IfExpression::~IfExpression() {

}

TypeSymbol* IfExpression::Type() const {
	TypeSymbol* type = nodes[0]->Type();

	for (UInt i = 1; i < nodes.Size(); i++) {
		if (type != nodes[i]->Type()) {
			ErrorLog::Error(LogMessage("error.type.if"), file);
		}
	}

	return type;
}

CompiledNode IfExpression::Compile(OldCompileInfo& info) {
	CompiledNode cn;

	// Setup return value
	cn.size = Type()->Size();

	List<UInt> jumps;

	info.stack.PushExpr(cn.size, cn);
	const UInt frame = info.stack.frame; 

	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	Ptr<StackExpression> sn = new StackExpression(info.stack.top);
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
		cn.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->File()).instructions);

		info.stack.PopExpr(frame, cn);

		// Add jump and label
		jumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));

		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		cn.instructions.Add(Instruction::Label(info.label++));
	}

	info.stack = stack;

	// Compile else
	cn.AddInstructions(CompileAssignment(sn, nodes.Last(), info, nodes.Last()->File()).instructions);

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

void IfExpression::IncludeScan(ParsingInfo& info) {
	for (Weak<Expression> node : nodes) {
		node->IncludeScan(info);
	}

	for (Weak<Condition> condition : conditions) {
		condition->IncludeScan(info);
	}
}

ScanResult IfExpression::Scan(ScanInfoStack& info) {
	ScanResult result;
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	TypeSymbol* const t = Type();
	Ptr<TypeExpression> type = t ? new TypeExpression(t->AbsoluteName()) : nullptr;

	// Scan values
	for (Weak<Expression> node : nodes) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;

		if (type) {
			ScanAssignment(type, node, info, node->File());
		}
	}

	// Scan conditions
	for (Weak<Condition> node : conditions) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;
	}

	info->scopeInfo = scopeInfo;
	return result;
}

NameList IfExpression::FindSideEffectScope(const bool assign) {
	NameList list = conditions[0]->GetSideEffectScope(assign);

	for (UInt i = 1; i < conditions.Size(); i++) {
		list = CombineSideEffects(list, conditions[i]->GetSideEffectScope(assign));
	}

	for (Weak<Expression> node : nodes) {
		list = CombineSideEffects(list, node->GetSideEffectScope(assign));
	}

	return list;
}

Ptr<Expression> IfExpression::Optimize(OptimizeInfo& info) {
	// Optimize nodes and conditions
	for (Ptr<Expression>& node : nodes) {
		Node::Optimize(node, info);
	}

	for (Ptr<Condition>& cond : conditions) {
		Node::Optimize(cond, info);
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
			Ptr<TypeConversion> cn = new TypeConversion(nodes[0]->scope, nodes[0]->File());
			cn->isExplicit = true;
			cn->expression = nodes[0];
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

StringBuilder IfExpression::ToMelon(const UInt indent) const {
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