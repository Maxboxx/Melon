#include "TemplateTypeSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

TemplateTypeSymbol::TemplateTypeSymbol(const FileInfo& file) : TypeSymbol(0, file) {
	
}

TemplateTypeSymbol::~TemplateTypeSymbol() {
	for (TemplateTypeSymbol* variant : templateVariants) {
		delete variant;
	}
}

void TemplateTypeSymbol::AddTemplateVariant(TemplateTypeSymbol* const sym) {
	sym->name = name;
	sym->parent = this;
	templateVariants.Add(sym);
}

TypeSymbol* TemplateTypeSymbol::TemplateArgument(const UInt index) const {
	return nullptr;
}

Symbol* TemplateTypeSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;

	const Scope& scope = scopeList[index];

	if (scope.name.Size() == 0 && scope.types && !scope.arguments) {
		for (TemplateTypeSymbol* const variant : templateVariants) {
			const List<ScopeList> types = scope.types.Get();

			if (types.Size() != templateArguments.Size()) continue;

			bool match = true;

			for (UInt i = 0; i < types.Size(); i++) {
				if (types[i] != templateArguments[i]) {
					match = false;
					break;
				}
			}

			if (match) {
				return variant->Find(scopeList, index + 1, file);
			}
		}

		FindError(scopeList, index, file);
		return nullptr;
	}
	else {
		return TypeSymbol::Find(scopeList, index, file);
	}
}
