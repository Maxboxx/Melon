#include "KiwiMemoryExpression.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiMemoryExpression::KiwiMemoryExpression(const Boxx::Int offset, const Symbols::NameList& type) : Expression(nullptr, FileInfo()) {
	this->mem  = MemoryLocation(offset);
	this->type = type;

	if (type == NameList()) {
		int i = 0;
	}
}

KiwiMemoryExpression::KiwiMemoryExpression(const MemoryLocation& mem, const Symbols::NameList& type) : Expression(nullptr, FileInfo()) {
	this->mem  = mem;
	this->type = type;

	if (type == NameList()) {
		int i = 0;
	}
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
