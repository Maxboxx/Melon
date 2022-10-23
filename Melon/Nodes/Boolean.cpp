#include "Boolean.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Boolean::Boolean(const FileInfo& file) : Expression(nullptr, file) {

}

Boolean::~Boolean() {

}

TypeSymbol* Boolean::Type() const {
	return (TypeSymbol*)SymbolTable::Bool;
}

bool Boolean::IsImmediate() const {
	return true;
}

Long Boolean::GetImmediate() const {
	return value ? 1 : 0;
}

CompiledNode Boolean::Compile(OldCompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(GetImmediate());
	node.size = 1;
	return node;
}

StringBuilder Boolean::ToMelon(const UInt indent) const {
	return String(value ? "true" : "false");
}
