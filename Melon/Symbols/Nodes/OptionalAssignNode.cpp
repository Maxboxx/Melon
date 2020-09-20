#include "OptionalAssignNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Nodes/StackNode.h"
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
	CompiledNode c2 = nodes[1]->Compile(info);
	c1.AddInstructions(c2.instructions);

	const ScopeList type1 = nodes[0]->Type();
	const ScopeList type2 = nodes[1]->Type();

	Symbol s = Symbol::Find(type1, nodes[0]->file);
	UInt compIndex = c1.instructions.Size();

	Instruction comp = Instruction(InstructionType::Eq, 1);
	comp.arguments.Add(c2.argument);
	comp.arguments.Add(Argument(0));
	c1.instructions.Add(comp);

	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(c1.argument);
	mov1.arguments.Add(Argument(1));
	c1.instructions.Add(mov1);

	Symbol argSym = s.Get(s.arguments[1], nodes[0]->file);
	Symbol argType = argSym.GetType(nodes[0]->file);

	const ScopeList typeName = argType.scope;

	List<ScopeList> typeArgs;
	typeArgs.Add(typeName);
	Symbol argAssign = Symbol::FindFunction(typeName.Add(Scope::Assign), typeArgs, nodes[0]->file);

	List<NodePtr> args;

	Pointer<StackNode> sn1 = new StackNode(c1.argument.mem.offset + 1);
	
	if (c1.argument.mem.reg.type == RegisterType::Register) {
		sn1->regIndex = c1.argument.mem.reg.index;
	}

	sn1->type = typeName;

	if (c1.argument.mem.reg.type == RegisterType::Register) {
		sn1->regIndex = c1.argument.mem.reg.index;
	}

	args.Add(sn1);

	Symbol s2 = Symbol::Find(type2, nodes[1]->file);

	Pointer<StackNode> sn2 = new StackNode(c2.argument.mem.offset + 1);

	if (c2.argument.mem.reg.type == RegisterType::Register) {
		sn2->regIndex = c2.argument.mem.reg.index;
	}

	sn2->type = s2.templateArgs[0];
	args.Add(sn2);

	info.important = important;
	c1.AddInstructions(argAssign.symbolNode->Compile(args, info).instructions);

	UInt jumpIndex = c1.instructions.Size();
	c1.instructions.Add(Instruction(InstructionType::Jmp));

	c1.instructions.Add(Instruction::Label(info.label));
	c1.instructions[compIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(c1.argument);
	mov2.arguments.Add(Argument(0));
	c1.instructions.Add(mov2);

	c1.instructions.Add(Instruction::Label(info.label));
	c1.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	info.important = important;

	return c1;
}
