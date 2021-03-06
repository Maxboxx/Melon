#include "DoNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "BreakNode.h"
#include "EmptyNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DoNode::DoNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

DoNode::~DoNode() {

}

UInt DoNode::GetSize() const {
	return nodes->GetSize();
}

bool DoNode::IsScope() const {
	return true;
}

CompiledNode DoNode::Compile(CompileInfo& info) {
	CompiledNode compiled;

	List<UInt> jumps;

	UInt top = info.stack.top;

	// Compile content
	for (const OptimizerInstruction& in : nodes->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scope wise break
		if (type != BreakNode::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		// Handle scope wise break
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

	// Add labels for scope breaks
	if (!jumps.IsEmpty()) {
		Instruction lbl = Instruction::Label(info.label);
		compiled.instructions.Add(lbl);

		for (const UInt jump : jumps) {
			compiled.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
	}

	info.stack.top = top;
	return compiled;
}

void DoNode::IncludeScan(ParsingInfo& info) {
	nodes->IncludeScan(info);
}

ScanResult DoNode::Scan(ScanInfoStack& info) {
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	ScanResult result = nodes->Scan(info);
	info->scopeInfo.ExitScope();
	return result;
}

ScopeList DoNode::FindSideEffectScope(const bool assign) {
	return nodes->GetSideEffectScope(assign);
}

NodePtr DoNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = nodes->Optimize(info)) nodes = node;

	if (IsEmpty(nodes) || !nodes->HasSideEffects()) {
		info.optimized = true;
		return new EmptyNode();
	}

	return nullptr;
}

StringBuilder DoNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "do\n";
	sb += String('\t').Repeat(indent + 1);
	sb += nodes->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}