#include "VariableSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"

#include "Melon/Nodes/RootNode.h"
#include "Melon/Nodes/Structs/StackPtr.h"

#include "Boxx/Map.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

VariableSymbol::VariableSymbol(const FileInfo& file) : Symbol(file) {

}

VariableSymbol::~VariableSymbol() {
	
}

bool VariableSymbol::HasAttribute(const VariableAttributes attribute) {
	return (this->attributes & attribute) != VariableAttributes::None;
}

Boxx::UInt VariableSymbol::Size() {
	return HasAttribute(VariableAttributes::Ref) ? StackPtr::ptrSize : Type()->Size();
}

TypeSymbol* VariableSymbol::Type() {
	return SymbolTable::Find<TypeSymbol>(type, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

VariableSymbol* VariableSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	VariableSymbol* const sym = new VariableSymbol(file);
	sym->attributes = attributes;
	sym->type = ReplaceTypeScope(Type(), replacement, file);
	return sym;
}
