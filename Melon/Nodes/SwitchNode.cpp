#include "SwitchNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

SwitchNode::SwitchNode(Symbols::Symbol* const scope, const FileInfo& file) : SwitchBaseNode<Statement>(scope, file) {
	expr = false;
}

SwitchNode::~SwitchNode() {
	
}

bool SwitchNode::IsScope() const {
	return true;
}

Statement SwitchNode::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
