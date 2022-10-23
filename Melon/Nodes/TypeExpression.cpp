#include "TypeExpression.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

TypeExpression::TypeExpression(const NameList& type) : Expression(nullptr, FileInfo()) {
	this->type = type;
}

TypeExpression::~TypeExpression() {

}

CompiledNode TypeExpression::Compile(OldCompileInfo& info) {
	return CompiledNode();
}

TypeSymbol* TypeExpression::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

StringBuilder TypeExpression::ToMelon(const UInt indent) const {
	return StringBuilder();
}
