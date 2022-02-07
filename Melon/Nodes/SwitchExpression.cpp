#include "SwitchExpression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

SwitchExpression::SwitchExpression(Symbols::Symbol* const scope, const FileInfo& file) : SwitchBaseNode<Expression>(scope, file) {
	expr = true;
}

SwitchExpression::~SwitchExpression() {
	
}

Symbols::TypeSymbol* SwitchExpression::Type() const {
	return SwitchType();
}

Ptr<Expression> SwitchExpression::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
