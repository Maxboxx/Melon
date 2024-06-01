#include "PtrSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

PtrSymbol::PtrSymbol(const FileInfo& file) : TemplateTypeSymbol(file) {

}

PtrSymbol::~PtrSymbol() {

}

TypeSymbol* PtrSymbol::PtrType() {
	return SymbolTable::Find<TypeSymbol>(this->type, AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

Kiwi::Type PtrSymbol::KiwiType() {
	Kiwi::Type type = PtrType()->KiwiType();
	type.pointers++;
	return type;
}

UInt PtrSymbol::Size() const {
	return Kiwi::KiwiProgram::ptrSize;
}

void PtrSymbol::UpdateSize() {
	size = 0;

	if (TypeSymbol* const type = PtrType()) {
		if (type->Size() == 0) {
			type->UpdateSize();
		}

		size += type->Size();
	}
}

PtrSymbol* PtrSymbol::InitializeSpecialize() {
	return new PtrSymbol(file);
}

void PtrSymbol::SpecializeTemplate(Symbol* initSym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) {
	PtrSymbol* sym = initSym->Cast<PtrSymbol>();
	TypeSymbol* type = PtrType();

	if (type == nullptr && this->type.HasTemplates()) {
		root->AddTemplateSpecialization(this->type, Parent()->AbsoluteName(), file, false);
		type = PtrType();
	}

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSym = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSym);
		s.value->SpecializeTemplate(newSym, replacement, root);
	}

	for (UInt i = 0; i < templateArguments.Count(); i++) {
		TypeSymbol* const type = TemplateArgument(i);

		if (templateArguments[i].IsTemplate() && type == replacement.GetValue(type)) {
			sym->templateArguments.Add(templateArguments[i]);
		}
		else {
			sym->templateArguments.Add(ReplaceTypeScope(type, replacement, file));
		}
	}

	sym->type = ReplaceTypeScope(type, replacement, file);

	if (sym->type.HasTemplates() && !sym->PtrType()) {
		SymbolTable::TemplateInfo info;
		info.file  = sym->file;
		info.name  = sym->type;
		info.scope = sym->Parent();
		SymbolTable::templateSymbols.Add(info);
	}
}
