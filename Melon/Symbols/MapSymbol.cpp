#include "MapSymbol.h"

#include "Symbol.h"
#include "ScopeList.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

#include "Melon/Errors.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

MapSymbol::MapSymbol(const FileInfo& file) : Symbol(file) {
	
}

MapSymbol::~MapSymbol() {
	for (const Pair<Scope, Symbol*>& symbol : symbols) {
		delete symbol.value;
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
