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

ClassSymbol* ClassSymbol::BaseClass() const {
	if (!baseClass) return nullptr;

	return SymbolTable::Find<ClassSymbol>(*baseClass, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

Symbol* ClassSymbol::FindSymbol(const NameList& nameList, const Boxx::UInt index, const FileInfo& file) {
	ErrorLog::AddMarker();

	if (Symbol* const sym = ClassStructBaseSymbol::FindSymbol(nameList, index, file)) {
		ErrorLog::RemoveMarker();
		return sym;
	}

	if (ClassSymbol* const base = BaseClass()) {
		ErrorLog::Revert();
		return base->FindSymbol(nameList, index, file);
	}

	ErrorLog::RemoveMarker();
	return nullptr;
}
