#include "VariableSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"

#include "Melon/Nodes/RootNode.h"

#include "Boxx/Map.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

VariableSymbol::VariableSymbol(const FileInfo& file) : Symbol(file) {

}

VariableSymbol::~VariableSymbol() {
	
}

TypeSymbol* VariableSymbol::Type() {
	return SymbolTable::Find<TypeSymbol>(type, Parent()->AbsoluteName(), file);
}

VariableSymbol* VariableSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	VariableSymbol* const sym = new VariableSymbol(file);
	sym->attributes = attributes;

	Map<TemplateSymbol*, ScopeList> templateTypes;

	for (const Pair<TypeSymbol*, TypeSymbol*>& pair : replacement) {
		if (TemplateSymbol* const type = pair.key->Cast<TemplateSymbol>()) {
			templateTypes.Add(type, type->type);
			type->type = pair.value->AbsoluteName();
		}
	}

	if (TypeSymbol* const type = Type()) {
		sym->type = SymbolTable::ReplaceTemplatesAbsolute(type->AbsoluteName(), file);
	}

	for (const Pair<TemplateSymbol*, ScopeList>& pair : templateTypes) {
		pair.key->type = pair.value;
	}

	return sym;
}
