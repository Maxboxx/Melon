#include "StackExpression.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StackExpression::StackExpression(const UInt stack) : Expression(nullptr, FileInfo()) {
	this->stack = stack;
}

StackExpression::~StackExpression() {

}

TypeSymbol* StackExpression::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode StackExpression::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = MemoryLocation(info.stack.Offset(stack));
	c.size = Type()->Size();
	return c;
}

StringBuilder StackExpression::ToMelon(const UInt indent) const {
	return StringBuilder();
}