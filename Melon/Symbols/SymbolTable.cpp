#include "SymbolTable.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

Pointer<ScopeSymbol> SymbolTable::symbols = new ScopeSymbol(FileInfo());

Symbol* SymbolTable::Find(const ScopeList& name, const FileInfo& file) {
	return symbols->Symbol::Find(name, file);
}

Symbol* SymbolTable::Contains(const ScopeList& name) {
	return symbols->Contains(name);
}
