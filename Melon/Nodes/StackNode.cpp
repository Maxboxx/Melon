#include "StackNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StackNode::StackNode(const UInt stack) : Node(ScopeList(), FileInfo()) {
	this->stack = stack;
}

StackNode::~StackNode() {

}

ScopeList StackNode::Type() const {
	return type;
}

CompiledNode StackNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = MemoryLocation(info.stack.Offset(stack));
	c.size = Symbols::Find(type, file).size;
	return c;
}

Mango StackNode::ToMango() const {
	return Mango();
}

StringBuilder StackNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}