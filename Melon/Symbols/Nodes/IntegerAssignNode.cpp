#include "IntegerAssignNode.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/Symbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;

using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Optimizing;

Ptr<Kiwi::Value> IntegerAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	return Node::CompileAssignmentSimple(operand1, operand2, info, operand1->File(), includeType);
}