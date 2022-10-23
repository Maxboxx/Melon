#include "OptionalToBooleanNode.h"

#include "Melon/Symbols/NameList.h"

#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalToBooleanNode::Compile(Weak<Expression> operand, OldCompileInfo& info) const {
	return operand->Compile(info);
}
