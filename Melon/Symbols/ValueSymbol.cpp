#include "ValueSymbol.h"

#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ValueSymbol::ValueSymbol(const FileInfo& file) : Symbol(file) {

}

ValueSymbol::~ValueSymbol() {

}

TypeSymbol* ValueSymbol::ValueType() {
	if (!type) return nullptr;

	return SymbolTable::Find<TypeSymbol>(*type, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

TypeSymbol* ValueSymbol::Type() {
	return parent->Cast<TypeSymbol>();
}

ValueSymbol* ValueSymbol::InitializeSpecialize() {
	ValueSymbol* const sym = new ValueSymbol(file);
	sym->value = value;
	return sym;
}

void ValueSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	ValueSymbol* sym = initSym->Cast<ValueSymbol>();

	if (TypeSymbol* valueType = ValueType()) {
		sym->type = ReplaceTypeScope(valueType, replacement, file);
	}
}
