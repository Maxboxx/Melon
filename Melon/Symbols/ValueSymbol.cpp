#include "ValueSymbol.h"

#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ValueSymbol::ValueSymbol(const FileInfo& file) : Symbol(file) {

}

ValueSymbol::~ValueSymbol() {

}

TypeSymbol* ValueSymbol::Type() {
	return parent->Cast<TypeSymbol>();
}

ValueSymbol* ValueSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	ValueSymbol* const sym = new ValueSymbol(file);
	sym->value = value;
	return sym;
}
