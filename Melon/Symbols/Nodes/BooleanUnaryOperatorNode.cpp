#include "BooleanUnaryOperatorNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

BooleanUnaryOperatorNode::BooleanUnaryOperatorNode(const InstructionType name) {
	op = name;
}

CompiledNode BooleanUnaryOperatorNode::Compile(const Expression& operand, CompileInfo& info) const {
	return CompiledNode();
}
