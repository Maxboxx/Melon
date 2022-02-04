#include "BooleanNotNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode BooleanNotNode::Compile(const _Expression_& operand, CompileInfo& info) const {
	CompiledNode c = operand->Compile(info);

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(c.argument);
	eq.arguments.Add(Argument(0));
	eq.arguments.Add(Argument(Register(info.index++)));
	c.instructions.Add(eq);

	c.argument = eq.arguments[2];
	c.size = 1;
	return c;
}

