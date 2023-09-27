#include "EnumAssignNode.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> EnumAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	return Node::CompileAssignmentSimple(operand1, operand2, info, operand1->File(), includeType);
}