#include "TemplateTypeSymbol.h"

#include "SymbolTable.h"

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

TypeSymbol* TemplateTypeSymbol::TemplateArgument(const UInt index) {
	if (index >= templateArguments.Size()) return nullptr;

	ScopeList arg = templateArguments[index];

	if (arg[0] == Scope("")) {
		return Symbol::Find<TypeSymbol>(arg[1], file);
	}
	else {
		return SymbolTable::FindAbsolute<TypeSymbol>(arg, file);
	}
}

Scope TemplateTypeSymbol::Name() {
	if (templateArguments.IsEmpty()) {
		return name;
	}
	else {
		Scope name = Scope();
		Scope empty = Scope("");
		List<ScopeList> args;

		for (UInt i = 0; i < templateArguments.Size(); i++) {
			if (templateArguments[i][0] == empty) {
				args.Add(templateArguments[i]);
			}
			else if (TypeSymbol* const type = TemplateArgument(i)) {
				args.Add(type->AbsoluteName());
			}
			else {
				args.Add(ScopeList::undefined);
			}
		}

		name.types = args;
		return name;
	}
}

Symbol* TemplateTypeSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;

	const Scope& scope = scopeList[index];

	if (scope.name.Size() == 0 && scope.types && !scope.arguments) {
		for (TemplateTypeSymbol* const variant : templateVariants) {
			const List<ScopeList> types = scope.types.Get();

			if (types.Size() != variant->templateArguments.Size()) continue;

			bool match = true;

			for (UInt i = 0; i < types.Size(); i++) {
				if (types[i] != variant->templateArguments[i]) {
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
