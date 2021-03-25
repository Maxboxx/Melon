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

IntegerSymbol* IntegerSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	IntegerSymbol* const sym = new IntegerSymbol(size, isSigned, file);
	
	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	return sym;
}
