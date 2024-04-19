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

bool VariableSymbol::HasAttribute(const VariableModifiers modifier) {
	return (this->modifiers & modifier) != VariableModifiers::None;
}

Boxx::UInt VariableSymbol::Size() {
	return HasAttribute(VariableModifiers::Ref) ? Kiwi::KiwiProgram::ptrSize : Type()->Size();
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

TypeSymbol* VariableSymbol::Type() {
	return SymbolTable::Find<TypeSymbol>(type, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

VariableSymbol* VariableSymbol::InitializeSpecialize() {
	VariableSymbol* const sym = new VariableSymbol(file);
	sym->modifiers = modifiers;
	return sym;
}

void VariableSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	VariableSymbol* sym = initSym->Cast<VariableSymbol>();
	TypeSymbol* type = Type();

	if (type == nullptr && this->type.HasTemplates()) {
		root->AddTemplateSpecialization(this->type, Parent()->AbsoluteName(), file, false);
		type = Type();
	}

	sym->type = ReplaceTypeScope(type, replacement, file);

	if (sym->type.HasTemplates() && !sym->Type()) {
		SymbolTable::TemplateInfo info;
		info.file  = sym->file;
		info.name  = sym->type;
		info.scope = sym->Parent();
		SymbolTable::templateSymbols.Add(info);
	}
}
