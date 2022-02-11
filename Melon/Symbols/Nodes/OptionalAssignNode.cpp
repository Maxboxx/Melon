#include "OptionalAssignNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Nodes/KiwiMemoryExpression.h"
#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = operand1->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = operand2->Compile(info);
	c1.AddInstructions(c2.instructions);
	c1.argument = OffsetArgument(c1.argument, frame, info);

	TypeSymbol* const type1 = operand1->Type();
	TypeSymbol* const type2 = operand2->Type();

	UInt compIndex = c1.instructions.Size();

	Instruction comp = Instruction(InstructionType::Eq, 1);
	comp.arguments.Add(c2.argument);
	comp.arguments.Add(Argument(0));
	c1.instructions.Add(comp);

	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(c1.argument);
	mov1.arguments.Add(Argument(1));
	c1.instructions.Add(mov1);
	c1.instructions.Last().important = important;

	Ptr<KiwiMemoryExpression> mn1 = new KiwiMemoryExpression(c1.argument.mem);
	mn1->mem.offset++;
	mn1->type = type1->Find<VariableSymbol>(Name::Value, operand1->File())->Type()->AbsoluteName();

	Ptr<KiwiMemoryExpression> mn2 = new KiwiMemoryExpression(c2.argument.mem);
	mn2->mem.offset++;
	mn2->type = type2->Find<VariableSymbol>(Name::Value, operand2->File())->Type()->AbsoluteName();

	info.important = important;
	c1.AddInstructions(Node::CompileAssignment(mn1, mn2, info, operand2->File()).instructions);

	UInt jumpIndex = c1.instructions.Size();
	c1.instructions.Add(Instruction(InstructionType::Jmp));

	c1.instructions.Add(Instruction::Label(info.label));
	c1.instructions[compIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(c1.argument);
	mov2.arguments.Add(Argument(0));
	c1.instructions.Add(mov2);
	c1.instructions.Last().important = important;

	c1.instructions.Add(Instruction::Label(info.label));
	c1.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	info.important = important;

	return c1;
}
