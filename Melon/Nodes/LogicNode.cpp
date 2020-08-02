#include "LogicNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

LogicNode::LogicNode(const ScopeList& scope, const TokenType type, const FileInfo& file) : BinaryOperatorNode(scope, Scope(), file) {
	this->type = type;
}

LogicNode::~LogicNode() {

}

ScopeList LogicNode::Type() const {
	return ScopeList().Add(Scope::Bool);
}

Scope LogicNode::GetOperator() const {
	switch (type) {
		case TokenType::Or:   return Scope::Or;
		case TokenType::And:  return Scope::And;
		case TokenType::Xor:  return Scope::Xor;
		case TokenType::Nor:  return Scope::Nor;
		case TokenType::Nand: return Scope::Nand;
		case TokenType::Xnor: return Scope::Xnor;
	}

	return Scope("logic");
}

CompiledNode LogicNode::Compile(CompileInfo& info) {
	CompiledNode c;

	List<NodePtr> node;
	node.Add(node1);

	List<UInt> lbls1, lbls2;
	UInt lbl1, lbl2;

	Argument arg = Argument(Register(info.index++));

	CompiledNode c1 = Symbol::FindMethod(node1->Type().Add(Scope::Bool), List<ScopeList>(), node1->file).node->Compile(node, info);
	c.AddInstructions(c1.instructions);

	if (type == TokenType::Or || type == TokenType::And || type == TokenType::Nor || type == TokenType::Nand) {
		Instruction eq = Instruction((type == TokenType::Or || type == TokenType::Nand) ? InstructionType::Eq : InstructionType::Ne, 1);
		eq.arguments.Add(c1.argument);
		eq.arguments.Add(0);

		lbls1.Add(c.instructions.Size());
		c.instructions.Add(eq);

		if (type == TokenType::Or || type == TokenType::And) {
			Instruction mov = Instruction(InstructionType::Mov, 1);
			mov.arguments.Add(arg);
			mov.arguments.Add(c1.argument);
			c.instructions.Add(mov);

			Instruction jmp = Instruction(InstructionType::Jmp);
			lbls2.Add(c.instructions.Size());
			c.instructions.Add(jmp);
		}
	}

	if (type == TokenType::Or || type == TokenType::And) {
		lbl1 = info.label++;
		c.instructions.Add(Instruction::Label(lbl1));
	}

	node[0] = node2;
	CompiledNode c2 = Symbol::FindMethod(node2->Type().Add(Scope::Bool), List<ScopeList>(), node2->file).node->Compile(node, info);
	c.AddInstructions(c2.instructions);

	if (type == TokenType::Or || type == TokenType::And) {
		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(arg);
		mov.arguments.Add(c2.argument);
		c.instructions.Add(mov);
	}
	else if (type == TokenType::Nor || type == TokenType::Nand) {
		Instruction eq = Instruction(type == TokenType::Nor ? InstructionType::Ne : InstructionType::Eq, 1);
		eq.arguments.Add(c2.argument);
		eq.arguments.Add(0);

		lbls1.Add(c.instructions.Size());
		c.instructions.Add(eq);

		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(arg);
		mov.arguments.Add(Argument(type == TokenType::Nor ? 1 : 0));
		c.instructions.Add(mov);

		Instruction jmp = Instruction(InstructionType::Jmp);
		lbls2.Add(c.instructions.Size());
		c.instructions.Add(jmp);

		lbl1 = info.label++;
		c.instructions.Add(Instruction::Label(lbl1));

		Instruction mov2 = Instruction(InstructionType::Mov, 1);
		mov2.arguments.Add(arg);
		mov2.arguments.Add(type == TokenType::Nor ? 0 : 1);
		c.instructions.Add(mov2);
	}
	else if (type == TokenType::Xor || type == TokenType::Xnor) {
		Instruction cmp = Instruction(type == TokenType::Xor ? InstructionType::Ne : InstructionType::Eq, 1);
		cmp.arguments.Add(c1.argument);
		cmp.arguments.Add(c2.argument);
		cmp.arguments.Add(arg);
		c.instructions.Add(cmp);
	}

	if (type != TokenType::Xor && type != TokenType::Xnor) {
		lbl2 = info.label++;
		c.instructions.Add(Instruction::Label(lbl2));

		for (const UInt i : lbls1)
			c.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, lbl1));

		for (const UInt i : lbls2)
			c.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, lbl2));
	}

	c.argument = arg;
	return c;
}

Set<ScanType> LogicNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = BinaryOperatorNode::Scan(info);

	Symbol::FindMethod(node1->Type().Add(Scope::Bool), List<ScopeList>(), node1->file);
	Symbol::FindMethod(node2->Type().Add(Scope::Bool), List<ScopeList>(), node2->file);

	return scanSet;
}

Mango LogicNode::ToMango() const {
	const Scope op = GetOperator();

	Mango mango = Mango(op.ToString(), MangoType::List);
	mango.Add(node1->ToMango());
	mango.Add(node2->ToMango());
	return mango;
}