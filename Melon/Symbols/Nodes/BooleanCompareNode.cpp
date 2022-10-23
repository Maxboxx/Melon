#include "BooleanCompareNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BooleanCompareNode::BooleanCompareNode(const InstructionType op) {
	this->op = op;
}

CompiledNode BooleanCompareNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, OldCompileInfo& info) const {
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

	CompiledNode c2 = operand2->Compile(info);
	c1.AddInstructions(c2.instructions);

	if (info.stack.frame > frame) {
		if (c2.argument.type == ArgumentType::Memory) {
			if (c2.argument.mem.memptr.IsLeft() && c2.argument.mem.memptr.GetLeft().type == RegisterType::Stack) {
				c2.argument.mem.offset -= info.stack.frame - frame;
			}
		}

		info.stack.PopExpr(frame, c1);
	}

	Instruction comp = Instruction(op, 1);
	comp.arguments.Add(c1.argument);
	comp.arguments.Add(c2.argument);
	comp.arguments.Add(Argument(Register(info.index++)));
	c1.instructions.Add(comp);

	info.stack.top = top;
	c1.argument = comp.arguments[2];
	return c1;
}

