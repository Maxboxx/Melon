#include "RepeatStatement.h"

#include "BreakStatement.h"
#include "ContinueStatement.h"
#include "DoStatement.h"
#include "EmptyStatement.h"
#include "Statements.h"
#include "Condition.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RepeatStatement::RepeatStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

RepeatStatement::~RepeatStatement() {

}

UInt RepeatStatement::GetSize() const {
	return statements->GetSize();
}

bool RepeatStatement::IsScope() const {
	return true;
}

CompiledNode RepeatStatement::Compile(OldCompileInfo& info) {
	CompiledNode compiled;

	const UInt label = info.label++;
	compiled.instructions.Add(Instruction::Label(label));

	List<UInt> endJumps;
	List<UInt> condJumps;

	// Compile loop body
	for (const OptimizerInstruction& in : statements->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for break instructions
		if (
			type != BreakStatement::abortInstName &&
			type != BreakStatement::scopeBreakInstName &&
			type != BreakStatement::breakTrueInstName &&
			type != BreakStatement::breakFalseInstName &&
			type != ContinueStatement::continueInstName
		) {
			compiled.instructions.Add(in);
			continue;
		}

		// Compile multi break
		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		// Compile breaks
		else if (
			type == BreakStatement::abortInstName || 
			type == BreakStatement::breakTrueInstName || 
			type == BreakStatement::breakFalseInstName || 
			type == BreakStatement::scopeBreakInstName
		) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			endJumps.Add(compiled.instructions.Count());
			compiled.instructions.Add(jmp);
		}
		// Compile continue
		else if (type == ContinueStatement::continueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			condJumps.Add(compiled.instructions.Count());
			compiled.instructions.Add(jmp);
		}
	}

	// Add jumps and labels
	if (!condJumps.IsEmpty()) {
		const UInt condLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(condLbl));

		for (const UInt inst : condJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, condLbl));
		}
	}

	// Compile condition
	const UInt frame = info.stack.frame;
	CompiledNode cond = condition->Compile(info);
	info.stack.PopExpr(frame, cond);
	compiled.AddInstructions(cond.instructions);

	// Check if condition is true
	Instruction comp = Instruction(InstructionType::Eq, 1);
	comp.arguments.Add(cond.argument);
	comp.arguments.Add(Argument(0));
	comp.arguments.Add(Argument(ArgumentType::Label, label));
	compiled.instructions.Add(comp);

	// Add end label and jumps
	if (!endJumps.IsEmpty()) {
		const UInt endLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(endLbl));

		for (const UInt inst : endJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, endLbl));
		}
	}

	return compiled;
}

Ptr<Kiwi::Value> RepeatStatement::Compile(CompileInfo& info) {
	const String topLbl = info.NewLabel();

	LoopInfo loop;
	loop.continueLabel = info.NewLabel();
	loop.trueLabel     = info.NewLabel();
	loop.falseLabel    = loop.trueLabel;
	loop.endLabel      = loop.trueLabel;

	info.loops.Push(loop);

	info.NewInstructionBlock(topLbl);
	statements->Compile(info);
	info.NewInstructionBlock(loop.continueLabel);

	Ptr<Kiwi::Value> value = condition->Compile(info);
	info.AddInstruction(new Kiwi::IfInstruction(value, nullptr, topLbl));

	info.NewInstructionBlock(loop.endLabel);
	info.loops.Pop();

	return nullptr;
}

void RepeatStatement::IncludeScan(ParsingInfo& info) {
	statements->IncludeScan(info);
	condition->IncludeScan(info);
}

ScanResult RepeatStatement::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Loop);

	ScanResult result1 = statements->Scan(info);
	result1.SelfUseCheck(info, statements->File());

	ScanResult result2 = condition->Scan(info);
	result2.SelfUseCheck(info, condition->File());

	info->scopeInfo.ExitScope();
	info->scopeInfo = ScopeInfo::WeakBranchUnion(scopeInfo, info->scopeInfo);

	return result1 | result2;
}

NameList RepeatStatement::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(statements->GetSideEffectScope(assign), condition->GetSideEffectScope(assign));
}

Ptr<Statement> RepeatStatement::Optimize(OptimizeInfo& info) {
	Node::Optimize(statements, info);
	Node::Optimize(condition, info);

	// TODO: Remove break, continue, abort and similar
	if (condition->IsImmediate() && condition->GetImmediate() != 0) {
		if (IsEmpty(statements)) {
			info.optimized = true;
			return new EmptyStatement();
		}

		Ptr<DoStatement> dn = new DoStatement(statements->scope, statements->File());
		dn->statements = statements;
		info.optimized = true;
		return dn;
	}

	return nullptr;
}

StringBuilder RepeatStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "repeat\n";
	sb += String('\t').Repeat(indent + 1);
	sb += statements->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "until ";
	sb += condition->ToMelon(indent);
	return sb;
}