#include "SwitchExprNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

SwitchExprNode::SwitchExprNode(Symbols::Symbol* const scope, const FileInfo& file) : SwitchBaseNode<ExpressionNode>(scope, file) {
	expr = true;
}

SwitchExprNode::~SwitchExprNode() {
	
}

Symbols::TypeSymbol* SwitchExprNode::Type() const {
	return SwitchType();
}

Expression SwitchExprNode::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
