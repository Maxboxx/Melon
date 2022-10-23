#include "IntegerToBoolNode.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode IntegerToBoolNode::Compile(Weak<Expression> operand, OldCompileInfo& info) const {
	CompiledNode c = operand->Compile(info);

	Instruction ne = Instruction(InstructionType::Ne, c.size);
	ne.arguments.Add(c.argument);
	ne.arguments.Add(Argument(0));
	ne.arguments.Add(Argument(Register(info.index++)));
	ne.sizes[2] = 1;
	c.instructions.Add(ne);

	c.argument = ne.arguments[2];
	return c;
}

