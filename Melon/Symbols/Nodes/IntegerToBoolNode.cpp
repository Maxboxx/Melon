#include "IntegerToBoolNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode IntegerToBoolNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c = nodes[0]->Compile(info);

	Instruction ne = Instruction(InstructionType::Ne, c.size);
	ne.arguments.Add(c.argument);
	ne.arguments.Add(Argument(0));
	ne.arguments.Add(Argument(Register(info.index++)));
	ne.sizes[2] = 1;
	c.instructions.Add(ne);

	c.argument = ne.arguments[2];
	return c;
}