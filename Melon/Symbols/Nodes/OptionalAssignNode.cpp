#include "OptionalAssignNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> OptionalAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	return Node::CompileAssignmentSimple(operand1, operand2, info, operand1->File(), includeType);
}
