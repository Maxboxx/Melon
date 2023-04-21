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

ClassSymbol* ClassSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	ClassSymbol* const sym = new ClassSymbol(file);
	return (ClassSymbol*)ClassStructBaseSymbol::SpecializeTemplate(replacement, root, sym);
}

UInt ClassSymbol::Size() const {
	return Kiwi::KiwiProgram::ptrSize;
}

Kiwi::Type ClassSymbol::KiwiType() {
	return Kiwi::Type(1, KiwiName());
}
