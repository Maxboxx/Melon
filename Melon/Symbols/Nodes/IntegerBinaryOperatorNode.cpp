#include "IntegerBinaryOperatorNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

IntegerBinaryOperatorNode::IntegerBinaryOperatorNode(const UByte size, const bool sign, const InstructionType name) {
	this->size = size;
	this->sign = sign;
	this->op = name;
}

CompiledNode IntegerBinaryOperatorNode::Compile(const _Expression_& operand1, const _Expression_& operand2, CompileInfo& info) const {
	const UInt top = info.stack.top;
	CompiledNode c1 = operand1->Compile(info);

	if (c1.argument.type == ArgumentType::Memory) {
		if (c1.argument.mem.memptr.IsLeft() && c1.argument.mem.memptr.GetLeft().type == RegisterType::Stack) {
			if (info.stack.frame - c1.argument.mem.offset > top) {
				info.stack.Push(c1.size);
			}
		}
	}

	const UInt frame = info.stack.frame;

	Argument arg1;

	if (!operand1->IsImmediate()) {
		arg1 = c1.argument;
	}
	else {
		Instruction mov;
		mov.type = InstructionType::Mov;

		arg1.type = ArgumentType::Register;
		arg1.reg.type = RegisterType::Register;
		arg1.reg.index = info.index++;
		c1.size = size;

		mov.sizes[0] = size;
		mov.sizes[1] = size;
		mov.signs[0] = operand1->Type()->Cast<IntegerSymbol>()->IsSigned();
		mov.signs[1] = operand2->Type()->Cast<IntegerSymbol>()->IsSigned();
		mov.arguments.Add(arg1);
		mov.arguments.Add(c1.argument);

		c1.instructions.Add(mov);
	}

	CompiledNode c2 = operand2->Compile(info);

	for (const OptimizerInstruction& instruction : c2.instructions) {
		c1.instructions.Add(instruction);
	}

	if (info.stack.frame > frame) {
		if (c2.argument.type == ArgumentType::Memory) {
			if (c2.argument.mem.memptr.IsLeft() && c2.argument.mem.memptr.GetLeft().type == RegisterType::Stack) {
				c2.argument.mem.offset -= info.stack.frame - frame;
			}
		}

		info.stack.PopExpr(frame, c1);
	}

	Instruction inst = Instruction(op);
	inst.sizes[0] = c1.size;
	inst.sizes[1] = operand2->IsImmediate() ? Math::Max(c1.size, c2.size) : c2.size;
	inst.signs[0] = operand1->Type()->Cast<IntegerSymbol>()->IsSigned();
	inst.signs[1] = operand2->Type()->Cast<IntegerSymbol>()->IsSigned();
	inst.arguments.Add(arg1);
	inst.arguments.Add(c2.argument);

	if (arg1.type == ArgumentType::Memory) {
		arg1 = Argument(Register(info.index++));
		inst.sizes[2] = Instruction::IsComp(op) ? 1 : Math::Max(inst.sizes[0], inst.sizes[1]);
		inst.signs[2] = sign;
		inst.arguments.Add(arg1);
	}
	else if (Instruction::IsComp(op) && arg1.type == ArgumentType::Register) {
		inst.sizes[2] = 1;
		inst.signs[2] = sign;
		inst.arguments.Add(arg1);
	}

	if (inst.arguments[1].type == ArgumentType::Register && inst.arguments[1].reg.type == RegisterType::Register) {
		if (inst.arguments.Size() < 3) {
			info.index--;
		}
	}

	c1.size = inst.arguments.Size() == 2 ? inst.sizes[0] : inst.sizes[2];
	c1.argument = arg1;
	c1.instructions.Add(inst);

	info.stack.top = top;
	return c1;
}