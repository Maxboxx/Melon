#include "ExpressionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ExpressionNode::ExpressionNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

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
