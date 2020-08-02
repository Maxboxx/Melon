#include "StackNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StackNode::StackNode(const Boxx::Int stack) : Node(ScopeList(), FileInfo()) {
	this->stack = stack;
}

StackNode::~StackNode() {

}

ScopeList StackNode::Type() const {
	return type;
}

CompiledNode StackNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = Argument(MemoryLocation(stack));
	c.size = Symbol::Find(type, file).size;
	return c;
}

Mango StackNode::ToMango() const {
	return Mango();
}