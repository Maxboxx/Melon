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

bool VariableSymbol::HasAttribute(const VariableModifiers modifier) {
	return (this->modifiers & modifier) != VariableModifiers::None;
}

Boxx::UInt VariableSymbol::Size() {
	return HasAttribute(VariableModifiers::Ref) ? StackPtr::ptrSize : Type()->Size();
}

Kiwi::Type VariableSymbol::KiwiType() {
	if (TypeSymbol* type = Type()) {
		Kiwi::Type kiwiType = type->KiwiType();

		if ((modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
			kiwiType.pointers++;
		}

		return kiwiType;
	}

	return Kiwi::Type();
}

String VariableSymbol::KiwiName() {
	if (Parent<TypeSymbol>()) {
		return Name().name;
	}

	return Symbol::KiwiName();
}

TypeSymbol* VariableSymbol::Type() {
	return SymbolTable::Find<TypeSymbol>(type, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

VariableSymbol* VariableSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	VariableSymbol* const sym = new VariableSymbol(file);
	sym->modifiers = modifiers;
	sym->type = ReplaceTypeScope(Type(), replacement, file);
	return sym;
}
