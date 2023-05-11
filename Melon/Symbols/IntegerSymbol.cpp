#include "IntegerSymbol.h"

#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

IntegerSymbol::IntegerSymbol(const UInt size, const bool isSigned, const FileInfo& file) : TypeSymbol(size, file) {
	this->isSigned = isSigned;
}

IntegerSymbol::~IntegerSymbol() {

}

bool IntegerSymbol::IsSigned() const {
	return isSigned;
}

String IntegerSymbol::KiwiName() {
	switch (size) {
		case 1: return isSigned ? "i8"  : "u8";
		case 2: return isSigned ? "i16" : "u16";
		case 4: return isSigned ? "i32" : "u32";
		case 8: return isSigned ? "i64" : "u64";
	}

	return Symbol::KiwiName();
}

IntegerSymbol* IntegerSymbol::InitializeSpecialize() {
	return new IntegerSymbol(size, isSigned, file);
}

void IntegerSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	IntegerSymbol* const sym = initSym->Cast<IntegerSymbol>();
	
	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSym = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSym);
		s.value->SpecializeTemplate(newSym, replacement, root);
	}
}
