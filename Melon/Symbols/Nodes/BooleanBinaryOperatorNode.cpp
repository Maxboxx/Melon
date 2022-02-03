#include "BooleanBinaryOperatorNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

BooleanBinaryOperatorNode::BooleanBinaryOperatorNode(const InstructionType op) {
	this->op = op;
}

CompiledNode BooleanBinaryOperatorNode::Compile(const Expression& operand1, const Expression& operand2, CompileInfo& info) const {
	return CompiledNode();
}

