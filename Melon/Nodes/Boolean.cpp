#include "Boolean.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Boolean::Boolean(const FileInfo& file) : Expression(nullptr, file) {

}

Boolean::~Boolean() {

}

TypeSymbol* Boolean::Type(TypeSymbol* expected) const {
	return (TypeSymbol*)SymbolTable::Bool;
}

Ptr<Kiwi::Value> Boolean::Compile(CompileInfo& info) {
	return new Kiwi::Integer(SymbolTable::Bool->KiwiType(), value ? 1 : 0);
}

StringBuilder Boolean::ToMelon(const UInt indent) const {
	return String(value ? "true" : "false");
}
