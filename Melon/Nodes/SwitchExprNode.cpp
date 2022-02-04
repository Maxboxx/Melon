#include "SwitchExprNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

SwitchExprNode::SwitchExprNode(Symbols::Symbol* const scope, const FileInfo& file) : SwitchBaseNode<Expression>(scope, file) {
	expr = true;
}

SwitchExprNode::~SwitchExprNode() {
	
}

Symbols::TypeSymbol* SwitchExprNode::Type() const {
	return SwitchType();
}

_Expression_ SwitchExprNode::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
