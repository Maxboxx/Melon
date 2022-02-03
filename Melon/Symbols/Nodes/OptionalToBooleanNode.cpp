#include "OptionalToBooleanNode.h"

#include "Melon/Symbols/NameList.h"

#include "Melon/Nodes/BooleanNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalToBooleanNode::Compile(const Expression& operand, CompileInfo& info) const {
	return operand->Compile(info);
}
