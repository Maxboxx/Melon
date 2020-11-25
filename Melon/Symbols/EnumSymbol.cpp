#include "EnumSymbol.h"

#include "SymbolTable.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

EnumSymbol::EnumSymbol(const FileInfo& file) : MapSymbol(file) {
	
}

EnumSymbol::~EnumSymbol() {
	
}

Symbol* EnumSymbol::Type() {
	return this;
}

bool EnumSymbol::IsType() const {
	return true;
}

UInt EnumSymbol::Size() const {
	return size;
}
