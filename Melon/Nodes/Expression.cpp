#include "Expression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Expression::Expression(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

TypeSymbol* Expression::Type() const {
	return nullptr;
}

List<TypeSymbol*> Expression::Types() const {
	if (TypeSymbol* const type = Type()) {
		List<TypeSymbol*> types{1};
		types.Add(type);
		return types;
	}

	return List<TypeSymbol*>(0);
}

Ptr<Expression> Expression::Optimize(OptimizeInfo& info) {
	return nullptr;
}

