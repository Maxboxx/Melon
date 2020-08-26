#include "GuardNode.h"

#include "BreakNode.h"
#include "StatementsNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

GuardNode::GuardNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

GuardNode::~GuardNode() {

}

CompiledNode GuardNode::Compile(CompileInfo& info) {
	CompiledNode compiled = cond->Compile(info);

	Instruction ne = Instruction(InstructionType::Ne, 1);
	ne.arguments.Add(compiled.argument);
	ne.arguments.Add(Argument(0));

	UInt jumpIndex = compiled.instructions.Size();
	compiled.instructions.Add(ne);

	List<UInt> jumps;

	for (const OptimizerInstruction& in : else_->Compile(info).instructions) {
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		if (type != BreakNode::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}

	if (!jumps.IsEmpty()) {
		Instruction lbl = Instruction::Label(info.label);
		compiled.instructions.Add(lbl);

		for (const UInt jump : jumps) {
			compiled.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
	}

	compiled.instructions.Add(Instruction::Label(info.label));
	compiled.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	return compiled;
}

void GuardNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	cond->IncludeScan(info);
	else_->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> GuardNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = cond->Scan(info);
	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBeforeEnter();
	info.Get().scopeInfo.EnterScope();

	for (const ScanType type : else_->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, else_->file));
		}
	}

	info.Get().scopeInfo.ExitScope();
	info.Get().scopeInfo.CombineAfterExit(scopeInfo);
	return scanSet;
}

Mango GuardNode::ToMango() const {
	Mango mango = Mango("guard", MangoType::Map);
	mango.Add("condition", cond->ToMango());
	mango.Add("else", else_->ToMango());
	return mango;
}
