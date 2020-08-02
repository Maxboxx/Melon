#include "BreakNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

BreakNode::BreakNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {
	
}

BreakNode::~BreakNode() {

}

CompiledNode BreakNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);

	if (!isBreak)
		in.instructionName = "abort";
	else if (breakBool)
		in.instructionName = "breaktrue";
	else
		in.instructionName = "breakfalse";

	c.instructions.Add(in);
	return c;
}

Mango BreakNode::ToMango() const {
	Mango m = Mango(isBreak ? "break" : "abort", MangoType::List);
	m.Add(Mango((Int)loops));

	if (isBreak)
		m.Add(Mango((bool)breakBool));

	return m;
}