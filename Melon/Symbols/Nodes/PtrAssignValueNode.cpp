#include "PtrAssignValueNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/KiwiVariable.h"
#include "Melon/Nodes/KiwiValue.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> PtrAssignValueNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	return nullptr;
}
