#include "KiwiVariable.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

KiwiVariable::KiwiVariable(const String& var, const NameList& type) : Expression(nullptr, FileInfo()) {
	this->var = var;
	this->type = type;
}

TypeSymbol* KiwiVariable::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode KiwiVariable::Compile(OldCompileInfo& info) {
	return CompiledNode();
}

Ptr<Kiwi::Value> KiwiVariable::Compile(CompileInfo& info) {
	return new Kiwi::Variable(var);
}

StringBuilder KiwiVariable::ToMelon(const UInt indent) const {
	return StringBuilder();
}
