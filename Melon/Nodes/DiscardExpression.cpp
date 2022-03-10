#include "DiscardExpression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

DiscardExpression::DiscardExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

DiscardExpression::~DiscardExpression() {

}

TypeSymbol* DiscardExpression::Type() const {
	return nullptr;
}

CompiledNode DiscardExpression::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder DiscardExpression::ToMelon(const Boxx::UInt indent) const {
	return "_";
}
