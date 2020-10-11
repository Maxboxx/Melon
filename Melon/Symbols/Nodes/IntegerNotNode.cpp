#include "IntegerNotNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode IntegerNotNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c = nodes[0]->Compile(info);

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