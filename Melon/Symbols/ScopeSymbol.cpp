#include "ScopeSymbol.h"

#include "Symbol.h"
#include "NameList.h"
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
	table->name   = Symbols::Name(String::ToString(scopes.Count()));
	scopes.Add(table);
	return table;
}

void ScopeSymbol::AddScope(ScopeSymbol* const scope) {
	scope->parent = this;
	scope->name   = Symbols::Name(String::ToString(scopes.Count()));
	scopes.Add(scope);
}

Symbol* ScopeSymbol::FindSymbol(const NameList& scopeList, const UInt index, const FileInfo& file) {
	static const Regex numReg = Regex("^%d+$");

	if (index >= scopeList.Size()) return this;

	if (numReg.Match(scopeList[index].name)) {
		const UInt i = scopeList[index].name.ToUInt();

		if (i < scopes.Count()) {
			return scopes[i]->FindSymbol(scopeList, index + 1, file);
		}
	}
	else {
		return MapSymbol::FindSymbol(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}

ScopeSymbol* ScopeSymbol::InitializeSpecialize() {
	return new ScopeSymbol(file);
}

void ScopeSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	ScopeSymbol* const sym = initSym->Cast<ScopeSymbol>();

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSym = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSym);
		s.value->SpecializeTemplate(newSym, replacement, root);
	}

	for (ScopeSymbol* const s : scopes) {
		ScopeSymbol* newSym = s->InitializeSpecialize();
		sym->AddScope(newSym);
		s->SpecializeTemplate(newSym, replacement, root);
	}
}
