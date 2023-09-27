#include "EnumSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumSymbol::EnumSymbol(const FileInfo& file) : TemplateTypeSymbol(file) {
	
}

EnumSymbol::~EnumSymbol() {
	
}

EnumSymbol* EnumSymbol::InitializeSpecialize() {
	EnumSymbol* const sym = new EnumSymbol(file);
	sym->values = values;
	return sym;
}

void EnumSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	EnumSymbol* const sym = initSym->Cast<EnumSymbol>();

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSymbol = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSymbol);
		s.value->SpecializeTemplate(newSymbol, replacement, root);
	}
}
