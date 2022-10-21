#include "BooleanUnaryOperatorNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

BooleanUnaryOperatorNode::BooleanUnaryOperatorNode(const InstructionType name) {
	op = name;
}

CompiledNode BooleanUnaryOperatorNode::Compile(Weak<Expression> operand, CompileInfo& info) const {
	return CompiledNode();
}
