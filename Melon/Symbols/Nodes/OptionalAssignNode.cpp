#include "OptionalAssignNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Nodes/MemoryNode.h"
#include "Melon/Nodes/BooleanNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalAssignNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = nodes[1]->Compile(info);
	c1.AddInstructions(c2.instructions);
	c1.argument = OffsetArgument(c1.argument, frame, info);

	TypeSymbol* const type1 = nodes[0]->Type();
	TypeSymbol* const type2 = nodes[1]->Type();

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

	Pointer<MemoryNode> mn1 = new MemoryNode(c1.argument.mem);
	mn1->mem.offset++;
	mn1->type = type1->Find<VariableSymbol>(Name::Value, nodes[0]->file)->Type()->AbsoluteName();

	Pointer<MemoryNode> mn2 = new MemoryNode(c2.argument.mem);
	mn2->mem.offset++;
	mn2->type = type2->Find<VariableSymbol>(Name::Value, nodes[1]->file)->Type()->AbsoluteName();

	info.important = important;
	c1.AddInstructions(Node::CompileAssignment(mn1, mn2, info, nodes[1]->file).instructions);

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
