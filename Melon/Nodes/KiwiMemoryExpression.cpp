#include "KiwiMemoryExpression.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiMemoryExpression::KiwiMemoryExpression(const Boxx::Int offset) : Expression(nullptr, FileInfo()) {
	this->mem = MemoryLocation(offset);
}

KiwiMemoryExpression::KiwiMemoryExpression(const MemoryLocation& mem) : Expression(nullptr, FileInfo()) {
	this->mem = mem;
}

KiwiMemoryExpression::~KiwiMemoryExpression() {

}

TypeSymbol* KiwiMemoryExpression::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode KiwiMemoryExpression::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = mem;
	c.size = SymbolTable::FindAbsolute<TypeSymbol>(type, file)->Size();
	return c;
}

StringBuilder KiwiMemoryExpression::ToMelon(const UInt indent) const {
	return StringBuilder();
}
