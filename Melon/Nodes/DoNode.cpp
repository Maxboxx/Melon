#include "DoNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "BreakNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DoNode::DoNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

DoNode::~DoNode() {

}

CompiledNode DoNode::Compile(CompileInfo& info) {
	CompiledNode compiled;

	List<UInt> jumps;

	for (const OptimizerInstruction& in : nodes->Compile(info).instructions) {
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

	return compiled;
}

void DoNode::IncludeScan(ParsingInfo& info) {
	nodes->IncludeScan(info);
}

Set<ScanType> DoNode::Scan(ScanInfoStack& info) {
	info.Get().scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	Set<ScanType> scanSet = nodes->Scan(info);
	info.Get().scopeInfo.ExitScope();
	return scanSet;
}

Mango DoNode::ToMango() const {
	Mango mango = nodes->ToMango();
	mango.SetLabel("do");
	return mango;
}