#include "IntegerBinaryOperatorNode.h"

#include "Melon/Symbols/Symbols.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

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

CompiledNode IntegerBinaryOperatorNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c1 = nodes[0]->Compile(info);

	Argument arg1;

	if (!nodes[0]->IsImmediate()) {
		arg1 = c1.argument;
	}
	else {
		Instruction mov;
		mov.type = InstructionType::Mov;

		arg1.type = ArgumentType::Register;
		arg1.reg.type = RegisterType::Register;
		arg1.reg.index = info.index++;

		mov.sizes[0] = size;
		mov.sizes[1] = c1.size;
		mov.signs[0] = Symbol::Find(nodes[0]->Type(), nodes[0]->file).sign;
		mov.signs[1] = Symbol::Find(nodes[1]->Type(), nodes[1]->file).sign;
		mov.arguments.Add(arg1);
		mov.arguments.Add(c1.argument);

		c1.instructions.Add(mov);
	}

	CompiledNode c2 = nodes[1]->Compile(info);

	for (const OptimizerInstruction& instruction : c2.instructions) {
		c1.instructions.Add(instruction);
	}

	Instruction inst = Instruction(op);
	inst.sizes[0] = c1.size;
	inst.sizes[1] = c2.size;
	inst.signs[0] = Symbol::Find(nodes[0]->Type(), nodes[0]->file).sign;
	inst.signs[1] = Symbol::Find(nodes[1]->Type(), nodes[1]->file).sign;
	inst.arguments.Add(arg1);
	inst.arguments.Add(c2.argument);

	if (arg1.type == ArgumentType::Memory) {
		arg1 = Argument(Register(info.index++));
		inst.sizes[2] = size;
		inst.signs[2] = sign;
		inst.arguments.Add(arg1);
	}

	if (inst.arguments[1].type == ArgumentType::Register && inst.arguments[1].reg.type == RegisterType::Register) {
		if (inst.arguments.Size() < 3) {
			info.index--;
		}
	}

	c1.size = size;
	c1.argument = arg1;
	c1.instructions.Add(inst);

	return c1;
}