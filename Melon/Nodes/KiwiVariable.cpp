#include "KiwiVariable.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiVariable::KiwiVariable(Ptr<Kiwi::Variable> var, const NameList& type) : Expression(nullptr, FileInfo()) {
	this->var = var;
	this->type = type;
}

TypeSymbol* KiwiVariable::Type(TypeSymbol* expected) const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

Ptr<Kiwi::Value> KiwiVariable::Compile(CompileInfo& info) {
	return var->Copy();
}

StringBuilder KiwiVariable::ToMelon(const UInt indent) const {
	return StringBuilder();
}
