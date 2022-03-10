#include "BooleanAssignNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

CompiledNode BooleanAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = operand1->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = operand2->Compile(info);

	OptimizerInstruction mov = Instruction(InstructionType::Mov, 1);
	mov.important = important;
	mov.instruction.arguments.Add(OffsetArgument(c1.argument, frame, info));
	mov.instruction.arguments.Add(c2.argument);

	c1.AddInstructions(c2.instructions);
	c1.instructions.Add(mov);

	info.important = important;

	return c1;
}