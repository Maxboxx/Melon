#include "CallExpression.h"

using namespace Boxx;
using namespace Kiwi;

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
	FunctionSymbol* const f = GetFunc();

	List<TypeSymbol*> types;

	if (f == nullptr) {
		types.Add(nullptr);
		return types;
	}

	if (IsInit()) {
		types.Add(expression->Type());
	}
	else for (UInt i = 0; i < f->returnValues.Size(); i++) {
		types.Add(f->ReturnType(i));
	}

	if (types.IsEmpty()) {
		types.Add(nullptr);
	}

	return types;
}

