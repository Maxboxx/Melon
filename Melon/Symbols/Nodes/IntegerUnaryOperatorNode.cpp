#include "IntegerUnaryOperatorNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

IntegerUnaryOperatorNode::IntegerUnaryOperatorNode(const UByte size, const InstructionType name) {
	this->size = size;
	this->op = name;
}

CompiledNode IntegerUnaryOperatorNode::Compile(const Expression& operand, CompileInfo& info) const {
	CompiledNode c1 = operand->Compile(info);

	if (c1.argument.type != ArgumentType::Register) {
		Argument reg = Argument(Register(info.index++));

		Instruction mov = Instruction(InstructionType::Mov, size);
		mov.arguments.Add(reg);
		mov.arguments.Add(c1.argument);

		c1.instructions.Add(mov);
		c1.argument = reg;
	}

	Instruction in = Instruction(op, size);
	in.arguments.Add(c1.argument);

	c1.instructions.Add(in);

	return c1;
}

