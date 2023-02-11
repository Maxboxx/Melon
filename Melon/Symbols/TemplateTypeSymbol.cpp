#include "TemplateTypeSymbol.h"

#include "SymbolTable.h"
#include "TemplateSymbol.h"

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
	if (index >= templateArguments.Count()) return nullptr;

	Symbols::NameList arg = templateArguments[index];

	if (arg[0].IsEmpty()) {
		return Symbol::Find<TypeSymbol>(arg[1], file);
	}
	else {
		return SymbolTable::FindAbsolute<TypeSymbol>(arg, file);
	}
}

bool TemplateTypeSymbol::CanBeDeduced(TemplateTypeSymbol* const type) {
	if (Parent() != type->Parent()) return false;
	if (templateArguments.Count() != type->templateArguments.Count()) return false;

	for (UInt i = 0; i < templateArguments.Count(); i++) {
		TypeSymbol* const arg1 = TemplateArgument(i);
		TypeSymbol* const arg2 = type->TemplateArgument(i);
		
		if (!arg1 || !arg2) return false;
		if (arg1->Is<TemplateSymbol>()) continue;

		if (TemplateTypeSymbol* const template1 = arg1->Cast<TemplateTypeSymbol>()) {
			if (TemplateTypeSymbol* const template2 = arg2->Cast<TemplateTypeSymbol>()) {
				if (!template1->CanBeDeduced(template2)) {
					return false;
				}
			}
			else {
				return false;
			}
		}
		else if (arg1 != arg2) {
			return false;
		}
	}

	return true;
}

Symbols::Name TemplateTypeSymbol::Name() {
	if (templateArguments.IsEmpty()) {
		return name;
	}
	else {
		Symbols::Name name = Symbols::Name();
		List<Symbols::NameList> args;

		for (UInt i = 0; i < templateArguments.Count(); i++) {
			if (templateArguments[i][0].IsEmpty()) {
				args.Add(templateArguments[i]);
			}
			else if (TypeSymbol* const type = TemplateArgument(i)) {
				args.Add(type->AbsoluteName());
			}
			else {
				args.Add(Symbols::NameList::undefined);
			}
		}

		name.types = args;
		return name;
	}
}

bool TemplateTypeSymbol::IsNotSpecialized() {
	for (UInt i = 0; i < templateArguments.Count(); i++) {
		if (TypeSymbol* const type = TemplateArgument(i)) {
			if (type->Is<TemplateSymbol>()) {
				return true;
			}
		}
	}

	return Symbol::IsNotSpecialized();
}

void TemplateTypeSymbol::SetTemplateValues(Symbol* const symbol) {
	TemplateTypeSymbol* const sym = symbol->Cast<TemplateTypeSymbol>();

	if (!sym) {
		// TODO: error?
		return;
	}

	if (templateArguments.Count() != sym->templateArguments.Count()) {
		// TODO: error?
		return;
	}

	for (UInt i = 0; i < templateArguments.Count(); i++) {
		TypeSymbol* const type1 = TemplateArgument(i);
		TypeSymbol* const type2 = sym->TemplateArgument(i);

		if (type1 && type2) {
			if (TemplateSymbol* const t = type1->Cast<TemplateSymbol>()) {
				t->type = type2->AbsoluteName();
			}
		}
	}

	Symbol::SetTemplateValues(symbol);
}

Symbol* TemplateTypeSymbol::FindSymbol(const NameList& nameList, const UInt index, const FileInfo& file) {
	if (index >= nameList.Size()) return this;

	const Symbols::Name& scope = nameList[index];

	if (scope.name.Length() == 0 && scope.types && !scope.arguments) {
		for (TemplateTypeSymbol* const variant : templateVariants) {
			const List<NameList> types = *scope.types;

			if (types.Count() != variant->templateArguments.Count()) continue;

			bool match = true;

			for (UInt i = 0; i < types.Count(); i++) {
				if (types[i] != variant->templateArguments[i]) {
					match = false;
					break;
				}
			}

			if (match) {
				return variant->FindSymbol(nameList, index + 1, file);
			}
		}
		
		FindError(nameList, index, file);
		return nullptr;
	}
	else {
		return TypeSymbol::FindSymbol(nameList, index, file);
	}
}
