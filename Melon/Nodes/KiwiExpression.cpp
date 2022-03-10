#include "KiwiExpression.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiExpression::KiwiExpression(const Argument& arg) : Expression(nullptr, FileInfo()) {
	argument = arg;
}

KiwiExpression::~KiwiExpression() {

}

TypeSymbol* KiwiExpression::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode KiwiExpression::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = argument;
	c.size = Type()->Size();
	return c;
}

StringBuilder KiwiExpression::ToMelon(const UInt indent) const {
	return StringBuilder();
}
