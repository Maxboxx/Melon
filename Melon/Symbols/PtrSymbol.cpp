#include "PtrSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

PtrSymbol::PtrSymbol(const FileInfo& file) : TypeSymbol(Kiwi::KiwiProgram::ptrSize, file) {

}

PtrSymbol::~PtrSymbol() {

}

TypeSymbol* PtrSymbol::PtrType() {
	return SymbolTable::FindAbsolute<TypeSymbol>(this->type, file);
}

Kiwi::Type PtrSymbol::KiwiType() {
	return Kiwi::Type(1, PtrType()->KiwiName());
}

TypeSymbol* PtrSymbol::InitializeSpecialize() {
	return new PtrSymbol(file);
}

void PtrSymbol::SpecializeTemplate(Symbol* initSym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) {
	PtrSymbol* sym = initSym->Cast<PtrSymbol>();
	TypeSymbol* type = PtrType();

	if (type == nullptr && this->type.HasTemplates()) {
		root->AddTemplateSpecialization(this->type, Parent()->AbsoluteName(), file, false);
		type = PtrType();
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
