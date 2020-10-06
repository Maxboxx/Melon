#include "StackNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StackNode::StackNode(const Boxx::Int stack) : Node(ScopeList(), FileInfo()) {
	this->stack = stack;
}

StackNode::StackNode(const Boxx::UInt regIndex, const Boxx::Int stack) : Node(ScopeList(), FileInfo()) {
	this->regIndex = regIndex;
	this->stack = stack;
}

StackNode::StackNode(const MemoryLocation& mem) : Node(ScopeList(), FileInfo()) {
	if (mem.reg.type == RegisterType::Register) {
		this->regIndex = mem.reg.index;
	}

	this->stack = mem.offset;
}

StackNode::~StackNode() {

}

ScopeList StackNode::Type() const {
	return type;
}

CompiledNode StackNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = Argument(MemoryLocation(stack));

	if (regIndex) {
		c.argument.mem.reg = Register((UInt)regIndex);
	}

	c.size = Symbol::Find(type, file).size;
	return c;
}

Mango StackNode::ToMango() const {
	return Mango();
}

StringBuilder StackNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}