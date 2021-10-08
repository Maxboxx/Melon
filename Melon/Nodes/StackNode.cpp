#include "StackNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StackNode::StackNode(const UInt stack) : ExpressionNode(nullptr, FileInfo()) {
	this->stack = stack;
}

StackNode::~StackNode() {

}

TypeSymbol* StackNode::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode StackNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = MemoryLocation(info.stack.Offset(stack));
	c.size = Type()->Size();
	return c;
}

StringBuilder StackNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}