#include "IntegerConvertNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode IntegerConvertNode::Compile(Weak<Expression> operand, CompileInfo& info) const {
	CompiledNode cn = operand->Compile(info);

	if (size >= targetSize || cn.argument.type == ArgumentType::Number) {
		cn.size = targetSize;
	}
	else if (cn.argument.type == ArgumentType::Register) {
		Instruction mov = Instruction(InstructionType::Mov);
		mov.arguments.Add(cn.argument);
		mov.arguments.Add(cn.argument);
		mov.sizes[0] = targetSize;
		mov.sizes[1] = size;
		mov.signs[0] = sign;
		mov.signs[1] = sign;

		cn.instructions.Add(mov);
		cn.size = targetSize;
	}
	else if (cn.argument.type == ArgumentType::Memory) {
		Argument arg = Argument(Register(info.index++));

		Instruction mov = Instruction(InstructionType::Mov);
		mov.arguments.Add(arg);
		mov.arguments.Add(cn.argument);
		mov.sizes[0] = targetSize;
		mov.sizes[1] = size;
		mov.signs[0] = sign;
		mov.signs[1] = sign;

		cn.instructions.Add(mov);
		cn.argument = arg;
		cn.size = targetSize;
	}

	return cn;
}

