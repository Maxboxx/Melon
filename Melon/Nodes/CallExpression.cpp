#include "CallExpression.h"

#include "BaseCallNode.hpp"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CallExpression::CallExpression(Symbols::Symbol* const scope, const FileInfo& file) : BaseCallNode<Expression>(scope, file) {
	isStatement = false;
}

CallExpression::~CallExpression() {

}

TypeSymbol* CallExpression::Type() const {
	return Types()[0];
}

List<TypeSymbol*> CallExpression::Types() const {
	return GetReturnTypes();
}

