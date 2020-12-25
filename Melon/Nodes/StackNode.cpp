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

Symbol* StackNode::Type() const {
	return SymbolTable::FindAbsolute(type, file);
}

CompiledNode StackNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = MemoryLocation(info.stack.Offset(stack));

	/* TODO: node
	c.size = Symbols::Find(type, file).size;
	*/

	return c;
}

StringBuilder StackNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}