#include "IntegerNotNode.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode IntegerNotNode::Compile(Weak<Expression> operand, OldCompileInfo& info) const {
	CompiledNode c = operand->Compile(info);

	Instruction eq = Instruction(InstructionType::Eq, c.size);
	eq.arguments.Add(c.argument);
	eq.arguments.Add(Argument(0));
	eq.arguments.Add(Argument(Register(info.index++)));
	eq.sizes[2] = 1;
	c.instructions.Add(eq);

	c.argument = eq.arguments[2];
	c.size = 1;
	return c;
}
