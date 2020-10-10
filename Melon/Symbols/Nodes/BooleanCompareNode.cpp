#include "BooleanCompareNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BooleanCompareNode::BooleanCompareNode(const InstructionType op) {
	this->op = op;
}

CompiledNode BooleanCompareNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c1 = nodes[0]->Compile(info);
	CompiledNode c2 = nodes[1]->Compile(info);
	c1.AddInstructions(c2.instructions);

	Instruction comp = Instruction(op, 1);
	comp.arguments.Add(c1.argument);
	comp.arguments.Add(c2.argument);
	comp.arguments.Add(Argument(Register(info.index++)));
	c1.instructions.Add(comp);

	c1.argument = comp.arguments[2];
	return c1;
}