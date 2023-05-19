#include "MatchExpression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

MatchExpression::MatchExpression(Symbols::Symbol* const scope, const FileInfo& file) : MatchBaseNode<Expression>(scope, file) {
	
}

MatchExpression::~MatchExpression() {
	
}

Symbols::TypeSymbol* MatchExpression::Type() const {
	return SwitchType();
}

Ptr<Expression> MatchExpression::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
