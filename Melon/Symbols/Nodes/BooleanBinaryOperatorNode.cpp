#include "BooleanBinaryOperatorNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

BooleanBinaryOperatorNode::BooleanBinaryOperatorNode(const InstructionType name) {
	op = name;
}

CompiledNode BooleanBinaryOperatorNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	return CompiledNode();
}
