#include "KiwiValue.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiValue::KiwiValue(Ptr<Kiwi::Value> value, const Symbols::NameList& type) : Expression(nullptr, FileInfo()) {
	this->value = value;
	this->type  = type;
}

KiwiValue::~KiwiValue() {

}

TypeSymbol* KiwiValue::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode KiwiValue::Compile(OldCompileInfo& info) {
	return CompiledNode();
}

Ptr<Kiwi::Value> KiwiValue::Compile(CompileInfo& info) {
	return value->CopyValue();
}

StringBuilder KiwiValue::ToMelon(const UInt indent) const {
	return StringBuilder();
}
