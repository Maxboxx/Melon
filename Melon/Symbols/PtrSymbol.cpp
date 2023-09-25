#include "PtrSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

PtrSymbol::PtrSymbol(const FileInfo& file) : TypeSymbol(Kiwi::KiwiProgram::ptrSize, file) {

}

PtrSymbol::~PtrSymbol() {

}

Kiwi::Type PtrSymbol::KiwiType() {
	return Kiwi::Type(1, SymbolTable::FindAbsolute(type, file)->KiwiName());
}

TypeSymbol* PtrSymbol::InitializeSpecialize() {
	return new PtrSymbol(file);
}
