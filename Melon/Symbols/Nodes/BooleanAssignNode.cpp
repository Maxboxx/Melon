#include "BooleanAssignNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

CompiledNode BooleanAssignNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);
	CompiledNode c2 = nodes[1]->Compile(info);

	OptimizerInstruction mov = Instruction(InstructionType::Mov, 1);
	mov.important = important;
	mov.instruction.arguments.Add(c1.argument);
	mov.instruction.arguments.Add(c2.argument);

	c1.AddInstructions(c2.instructions);
	c1.instructions.Add(mov);

	info.important = important;

	return c1;
}