#include "EnumSymbol.h"

#include "SymbolTable.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

EnumSymbol::EnumSymbol(const UInt size, const bool isSigned, const FileInfo& file) : IntegerSymbol(size, isSigned, file) {
	
}

EnumSymbol::~EnumSymbol() {
	
}

