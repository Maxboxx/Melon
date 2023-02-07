#include "NilValue.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

NilValue::NilValue(const FileInfo& file) : Expression(nullptr, file) {

}

NilValue::~NilValue() {

}

TypeSymbol* NilValue::Type() const {
	return SymbolTable::Nil;
}

bool NilValue::IsImmediate() const {
	return true;
}

Long NilValue::GetImmediate() const {
	return 0;
}

CompiledNode NilValue::Compile(OldCompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(0);
	node.size = 0;
	return node;
}

Ptr<Kiwi::Value> NilValue::Compile(CompileInfo& info) {
	return nullptr;
}

StringBuilder NilValue::ToMelon(const UInt indent) const {
	return "nil";
}