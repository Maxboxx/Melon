#include "BooleanToBooleanNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode BooleanToBooleanNode::Compile(Weak<Expression> operand, CompileInfo& info) const {
	return operand->Compile(info);
}
