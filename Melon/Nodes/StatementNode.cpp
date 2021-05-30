#include "StatementNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StatementNode::StatementNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

StatementNode::~StatementNode() {

}

bool StatementNode::IsScope() const {
	return false;
}

StatementNode* StatementNode::Optimize(OptimizeInfo& info) {
	return nullptr;
}
