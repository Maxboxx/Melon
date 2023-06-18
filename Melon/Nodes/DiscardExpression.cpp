#include "DiscardExpression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

DiscardExpression::DiscardExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

DiscardExpression::~DiscardExpression() {

}

TypeSymbol* DiscardExpression::Type(TypeSymbol* expected) const {
	return expected;
}

Ptr<Kiwi::Value> DiscardExpression::Compile(CompileInfo& info) {
	return nullptr;
}

StringBuilder DiscardExpression::ToMelon(const Boxx::UInt indent) const {
	return "_";
}
