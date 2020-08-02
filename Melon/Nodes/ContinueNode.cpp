#include "ContinueNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

ContinueNode::ContinueNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ContinueNode::~ContinueNode() {

}

CompiledNode ContinueNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);
	in.instructionName = "continue";
	c.instructions.Add(in);

	return c;
}

Mango ContinueNode::ToMango() const {
	return Mango("continue", (Int)loops);
}
