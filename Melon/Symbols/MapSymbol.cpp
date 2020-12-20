#include "MapSymbol.h"

#include "Symbol.h"
#include "ScopeList.h"

#include "Melon/Errors.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

MapSymbol::MapSymbol(const FileInfo& file) : Symbol(file) {
	
}

MapSymbol::~MapSymbol() {
	for (const Pair<Scope, Symbol*>& symbol : symbols) {
		delete symbol.value;
	}
}

template <class T>
T* MapSymbol::AddSymbol(const Scope& name, T* const symbol) {
	try {
		symbols.Add(name, symbol);
		symbol->name = name;
		symbol->parent = this;
		return symbol;
	}
	catch (MapKeyError& e) {
		delete symbol;
		ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + name.ToString() + SymbolError::RedefinitionEnd, symbol->file));
		return nullptr;
	}
}

Symbol* MapSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;

	const Scope& scope = scopeList[index];
	Symbol* sym = nullptr;

	if (symbols.Contains(scope, sym)) {
		return sym->Find(scopeList, index + 1, file);
	}
	else {
		FindError(scopeList, index, file);
		return nullptr;
	}
}
