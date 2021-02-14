#include "IntegerAssignNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/Symbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;

using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Optimizing;

IntegerAssignNode::IntegerAssignNode(const UByte size) {
	this->size = size;
}

CompiledNode IntegerAssignNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = nodes[1]->Compile(info);

	OptimizerInstruction mov = Instruction(InstructionType::Mov);
	mov.instruction.sizes[0] = c1.size;
	mov.instruction.sizes[1] = nodes[1]->IsImmediate() ? c1.size : c2.size;
	mov.instruction.signs[0] = nodes[0]->Type()->Cast<IntegerSymbol>()->IsSigned();
	mov.instruction.signs[1] = nodes[1]->Type()->Cast<IntegerSymbol>()->IsSigned();
	mov.important = important;
	mov.instruction.arguments.Add(OffsetArgument(c1.argument, frame, info));
	mov.instruction.arguments.Add(c2.argument);

	c1.AddInstructions(c2.instructions);
	c1.instructions.Add(mov);

	info.important = important;

	return c1;
}