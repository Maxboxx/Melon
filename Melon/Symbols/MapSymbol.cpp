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
	for (const Pair<String, Symbol*>& symbol : symbols) {
		delete symbol.value;
	}
}

Symbol* MapSymbol::AddSymbol(const String& name, Symbol* const symbol) {
	try {
		symbols.Add(name, symbol);
		symbol->name = name;
		symbol->parent = this;
		return symbol;
	}
	catch (MapKeyError& e) {
		delete symbol;
		ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + name + SymbolError::RedefinitionEnd, symbol->file));
		return nullptr;
	}
}

Symbol* MapSymbol::operator[](const String& name) const {
	Symbol* symbol;

	if (symbols.Contains(name, symbol)) {
		return symbol;
	}

	return nullptr;
}
