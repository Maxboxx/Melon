#include "MemoryNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

MemoryNode::MemoryNode(const Boxx::Int offset) : Node(nullptr, FileInfo()) {
	this->mem = MemoryLocation(offset);
}

MemoryNode::MemoryNode(const MemoryLocation& mem) : Node(nullptr, FileInfo()) {
	this->mem = mem;
}

MemoryNode::~MemoryNode() {

}

TypeSymbol* MemoryNode::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode MemoryNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = mem;

	/* TODO: node
	c.size = Symbols::Find(type, file).size;
	*/

	return c;
}

StringBuilder MemoryNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}