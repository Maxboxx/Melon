#include "VariableSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

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
	sym->type = replacement.GetValue(Type())->AbsoluteName();
	return sym;
}
