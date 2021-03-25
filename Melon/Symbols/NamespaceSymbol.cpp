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

NamespaceSymbol* NamespaceSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	NamespaceSymbol* const sym = new NamespaceSymbol(includedPath, file);

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	return sym;
}
