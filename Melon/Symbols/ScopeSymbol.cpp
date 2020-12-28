#include "ScopeSymbol.h"

#include "Symbol.h"
#include "ScopeList.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

#include "Melon/Errors.h"

#include "Boxx/Regex.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ScopeSymbol::ScopeSymbol(const FileInfo& file) : MapSymbol(file) {
	
}

ScopeSymbol::~ScopeSymbol() {
	for (const ScopeSymbol* const table : scopes) {
		delete table;
	}
}

ScopeSymbol* ScopeSymbol::AddScope(const FileInfo& file) {
	ScopeSymbol* const table = new ScopeSymbol(file);
	table->parent = this;
	table->name   = Scope(String::ToString(scopes.Size()));
	scopes.Add(table);
	return table;
}

void ScopeSymbol::AddScope(ScopeSymbol* const scope) {
	scope->parent = this;
	scope->name   = Scope(String::ToString(scopes.Size()));
	scopes.Add(scope);
}

Symbol* ScopeSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
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

ScopeSymbol* ScopeSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	ScopeSymbol* const sym = new ScopeSymbol(file);

	for (const Pair<Scope, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	for (ScopeSymbol* const s : scopes) {
		sym->AddScope(s->SpecializeTemplate(replacement, root));
	}

	return sym;
}
