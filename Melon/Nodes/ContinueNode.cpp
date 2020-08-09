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

Set<ScanType> ContinueNode::Scan(ScanInfoStack& info) const {
	info.Get().isBroken = true;
	info.Get().willNotBreak = false;
	return Set<ScanType>();
};

Mango ContinueNode::ToMango() const {
	return Mango("continue", (Int)loops);
}
