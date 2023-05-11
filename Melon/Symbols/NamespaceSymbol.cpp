#include "NamespaceSymbol.h"

#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

NamespaceSymbol::NamespaceSymbol(const String& includedPath, const FileInfo& file) : ScopeSymbol(file) {
	this->includedPath = includedPath;
}

NamespaceSymbol::~NamespaceSymbol() {

}

String NamespaceSymbol::IncludedPath() const {
	return includedPath;
}

NamespaceSymbol* NamespaceSymbol::InitializeSpecialize() {
	return new NamespaceSymbol(includedPath, file);
}

void NamespaceSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	NamespaceSymbol* const sym = initSym->Cast<NamespaceSymbol>();

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSym = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSym);
		s.value->SpecializeTemplate(newSym, replacement, root);
	}
}
