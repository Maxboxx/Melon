#include "EnumSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumSymbol::EnumSymbol(const UInt size, const bool isSigned, const FileInfo& file) : IntegerSymbol(size, isSigned, file) {
	
}

EnumSymbol::~EnumSymbol() {
	
}

EnumSymbol* EnumSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	EnumSymbol* const sym = new EnumSymbol(size, isSigned, file);
	sym->values = values;

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	return sym;
}
