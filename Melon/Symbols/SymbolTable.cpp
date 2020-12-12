#include "SymbolTable.h"

#include "Symbol.h"
#include "ScopeList.h"

#include "Melon/Errors.h"

#include "Boxx/Regex.h"

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

Symbol* SymbolTable::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	static const Regex numReg = Regex("^%d+$");

	if (index >= scopeList.Size()) return this;

	if (numReg.Match(scopeList[index].name)) {
		const UInt i = scopeList[index].name.ToUInt();

		if (i < scopes.Size()) {
			return scopes[i]->Find(scopeList, index + 1, file);
		}
	}
	else {
		return MapSymbol::Find(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}
