#include "RepeatNode.h"

#include "BreakNode.h"
#include "ContinueNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RepeatNode::RepeatNode(const Symbols::ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

RepeatNode::~RepeatNode() {

}

CompiledNode RepeatNode::Compile(CompileInfo& info) {
	CompiledNode compiled;

	const UInt label = info.label++;
	compiled.instructions.Add(Instruction::Label(label));

	List<UInt> endJumps;
	List<UInt> condJumps;

	for (const OptimizerInstruction& in : content->Compile(info).instructions) {
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		if (
			type != BreakNode::abortInstName &&
			type != BreakNode::scopeBreakInstName &&
			type != BreakNode::breakTrueInstName &&
			type != BreakNode::breakFalseInstName &&
			type != ContinueNode::continueInstName
		) {
			compiled.instructions.Add(in);
			continue;
		}

		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else if (
			type == BreakNode::abortInstName || 
			type == BreakNode::breakTrueInstName || 
			type == BreakNode::breakFalseInstName || 
			type == BreakNode::scopeBreakInstName
		) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			endJumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
		else if (type == ContinueNode::continueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			condJumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}

	if (!condJumps.IsEmpty()) {
		const UInt condLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(condLbl));

		for (const UInt inst : condJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, condLbl));
		}
	}

	CompiledNode cond = condition->Compile(info);
	compiled.AddInstructions(cond.instructions);

	Instruction comp = Instruction(InstructionType::Eq, 1);
	comp.arguments.Add(cond.argument);
	comp.arguments.Add(Argument(0));
	comp.arguments.Add(Argument(ArgumentType::Label, label));
	compiled.instructions.Add(comp);

	if (!endJumps.IsEmpty()) {
		const UInt endLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(endLbl));

		for (const UInt inst : endJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, endLbl));
		}
	}

	return compiled;
}

void RepeatNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	content->IncludeScan(info);
	condition->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> RepeatNode::Scan(ScanInfoStack& info) const {
	Set<ScanType> scanSet;

	UInt loopBreaks  = info.Get().loopBreakCount;
	UInt scopeBreaks = info.Get().scopeBreakCount;

	info.Get().EnterScope(true);

	for (const ScanType type : content->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, content->file));
		}
	}

	for (const ScanType type : condition->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, condition->file));
		}
	}

	info.Get().ExitScope(true);
	return scanSet;
}

Mango RepeatNode::ToMango() const {
	Mango mango = Mango("repeat", MangoType::Map);
	mango.Add("condition", condition->ToMango());
	mango.Add("content", content->ToMango());
	return mango;
}