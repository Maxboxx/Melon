#include "ClassSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ClassSymbol::ClassSymbol(const FileInfo& file) : ClassStructBaseSymbol(file) {

}

ClassSymbol::~ClassSymbol() {

}

ClassSymbol* ClassSymbol::InitializeSpecialize() {
	return new ClassSymbol(file);
}

UInt ClassSymbol::Size() const {
	return Kiwi::KiwiProgram::ptrSize;
}

Kiwi::Type ClassSymbol::KiwiType() {
	return Kiwi::Type(1, KiwiName());
}
