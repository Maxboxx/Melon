#include "SymbolTable.h"

#include "Symbol.h"
#include "ScopeList.h"

#include "Melon/Errors.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

SymbolTable SymbolTable::globalTable;

SymbolTable::SymbolTable(const FileInfo& file) : MapSymbol(file) {
	
}

SymbolTable::~SymbolTable() {
	for (const SymbolTable* const table : scopes) {
		delete table;
	}
}

SymbolTable* SymbolTable::AddScope(const FileInfo& file) {
	SymbolTable* const table = new SymbolTable(file);
	table->parent = this;
	scopes.Add(table);
	return table;
}

SymbolTable* SymbolTable::operator[](const UInt index) const {
	if (scopes.Size() > index) {
		return scopes[index];
	}

	return nullptr;
}

UInt SymbolTable::Index() const {
	return index;
}

Scope SymbolTable::Name() const {
	Scope name = Scope("");
	name.variant = index;
	return name;
}
