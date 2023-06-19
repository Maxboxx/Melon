#include "Character.h"

#include "Boxx/Math.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

Character::Character(const FileInfo& file) : Expression(nullptr, file) {

}

Character::~Character() {

}

TypeSymbol* Character::Type(TypeSymbol* expected) const {
	return (TypeSymbol*)SymbolTable::Char;
}

Ptr<Kiwi::Value> Character::Compile(CompileInfo& info) {
	return new Kiwi::Integer(SymbolTable::Char->KiwiType(), value);
}

StringBuilder Character::ToMelon(const UInt indent) const {
	StringBuilder builder;
	builder += "'";
	builder += (char)value;
	builder += "'";
	return builder;
}