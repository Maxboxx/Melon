#include "StackExpression.h"

using namespace Boxx;
using namespace KiwiOld;

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

StringBuilder StackExpression::ToMelon(const UInt indent) const {
	return StringBuilder();
}
