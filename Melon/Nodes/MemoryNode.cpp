#include "MemoryNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

MemoryNode::MemoryNode(const Boxx::Int offset) : Node(ScopeList(), FileInfo()) {
	this->mem = MemoryLocation(offset);
}

MemoryNode::MemoryNode(const MemoryLocation& mem) : Node(ScopeList(), FileInfo()) {
	this->mem = mem;
}

MemoryNode::~MemoryNode() {

}

ScopeList MemoryNode::Type() const {
	return type;
}

CompiledNode MemoryNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = mem;
	c.size = Symbols::Find(type, file).size;
	return c;
}

Mango MemoryNode::ToMango() const {
	return Mango();
}

StringBuilder MemoryNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}