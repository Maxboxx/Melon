#include "ExpressionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ExpressionNode::ExpressionNode(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

ExpressionNode::~ExpressionNode() {

}

TypeSymbol* ExpressionNode::Type() const {
	return nullptr;
}

List<TypeSymbol*> ExpressionNode::Types() const {
	if (TypeSymbol* const type = Type()) {
		List<TypeSymbol*> types{1};
		types.Add(type);
		return types;
	}

	return List<TypeSymbol*>(0);
}

Expression ExpressionNode::Optimize(OptimizeInfo& info) {
	return nullptr;
}

bool ExpressionNode::IsImmediate() const {
	return false;
}

Long ExpressionNode::GetImmediate() const {
	return 0;
}
