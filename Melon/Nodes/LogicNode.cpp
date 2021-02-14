#include "LogicNode.h"

#include "ConvertNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Symbols::Nodes;

LogicNode::LogicNode(Symbol* const scope, const TokenType type, const FileInfo& file) : BinaryOperatorNode(scope, Scope(), file) {
	this->type = type;
}

LogicNode::~LogicNode() {

}

TypeSymbol* LogicNode::Type() const {
	return (TypeSymbol*)SymbolTable::Bool;
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

CompiledNode LogicNode::CompileToBool(const NodePtr& node, CompileInfo& info) {
	Pointer<ConvertNode> convert = new ConvertNode(node->scope, node->file);
	convert->node = node;
	convert->type = ScopeList::Bool;
	convert->isExplicit = true;
	return convert->Compile(info);
}

CompiledNode LogicNode::CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) const {
	CompiledNode cn;
	cn.size = 1;

	List<UInt> jumps;

	CompiledNode operand1 = CompileToBool(node1, info);
	cn.AddInstructions(operand1.instructions);

	Instruction comp1 = Instruction(checkTrue ? InstructionType::Ne : InstructionType::Eq, 1);
	comp1.arguments.Add(operand1.argument);
	comp1.arguments.Add(Argument(0));
	jumps.Add(cn.instructions.Size());
	cn.instructions.Add(comp1);

	if (operand1.argument.type == ArgumentType::Register && operand1.argument.reg.type == RegisterType::Register) {
		info.index--;
	}

	CompiledNode operand2 = CompileToBool(node2, info);
	cn.AddInstructions(operand2.instructions);

	Instruction comp2 = Instruction(checkTrue ? InstructionType::Ne : InstructionType::Eq, 1);
	comp2.arguments.Add(operand2.argument);
	comp2.arguments.Add(Argument(0));
	jumps.Add(cn.instructions.Size());
	cn.instructions.Add(comp2);

	if (operand2.argument.type == ArgumentType::Register && operand2.argument.reg.type == RegisterType::Register) {
		info.index--;
	}

	cn.argument = Argument(Register(info.index++));

	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(cn.argument);
	mov1.arguments.Add(Argument(setTrue ? 0 : 1));
	cn.instructions.Add(mov1);

	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, info.label + 1));
	cn.instructions.Add(jmp);

	cn.instructions.Add(Instruction::Label(info.label));

	for (const UInt jump : jumps) {
		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	info.label++;

	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(cn.argument);
	mov2.arguments.Add(Argument(setTrue ? 1 : 0));
	cn.instructions.Add(mov2);

	cn.instructions.Add(Instruction::Label(info.label++));

	return cn;
}

CompiledNode LogicNode::CompileXor(CompileInfo& info, const bool checkEqual) const {
	CompiledNode cn;
	cn.argument = Argument(Register(info.index++));
	cn.size = 1;

	CompiledNode operand1 = CompileToBool(node1, info);
	cn.AddInstructions(operand1.instructions);

	CompiledNode operand2 = CompileToBool(node2, info);
	cn.AddInstructions(operand2.instructions);

	Instruction comp = Instruction(checkEqual ? InstructionType::Eq : InstructionType::Ne, 1);
	comp.arguments.Add(operand1.argument);
	comp.arguments.Add(operand2.argument);
	comp.arguments.Add(cn.argument);
	cn.instructions.Add(comp);

	if (operand2.argument.type == ArgumentType::Register && operand2.argument.reg.type == RegisterType::Register) {
		info.index--;

		if (operand1.argument.type == ArgumentType::Register && operand1.argument.reg.type == RegisterType::Register) {
			info.index--;
		}
	}

	return cn;
}

CompiledNode LogicNode::Compile(CompileInfo& info) {
	if (type == TokenType::Or) {
		return CompileAndOr(info, true, true);
	}
	else if (type == TokenType::And) {
		return CompileAndOr(info, false, false);
	}
	else if (type == TokenType::Xor) {
		return CompileXor(info, false);
	}
	else if (type == TokenType::Nor) {
		return CompileAndOr(info, true, false);
	}
	else if (type == TokenType::Nand) {
		return CompileAndOr(info, false, true);
	}
	else if (type == TokenType::Xnor) {
		return CompileXor(info, true);
	}

	return CompiledNode();
}

ScanResult LogicNode::Scan(ScanInfoStack& info) {
	ScanResult result1 = node1->Scan(info);
	result1.SelfUseCheck(info, node1->file);

	ScanResult result2 = node2->Scan(info);
	result2.SelfUseCheck(info, node2->file);

	Pointer<ConvertNode> convert1 = new ConvertNode(node1->scope, node1->file);
	convert1->node = node1;
	convert1->type = ScopeList::Bool;
	convert1->isExplicit = true;

	ScanResult result3 = convert1->Scan(info);

	Pointer<ConvertNode> convert2 = new ConvertNode(node2->scope, node2->file);
	convert2->node = node2;
	convert2->type = ScopeList::Bool;
	convert2->isExplicit = true;

	ScanResult result4 = convert2->Scan(info);

	return result1 | result2 | result3 | result4;
}

ScopeList LogicNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(node1->GetSideEffectScope(assign), node2->GetSideEffectScope(assign));
}

NodePtr LogicNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = node1->Optimize(info)) node1 = node;
	if (NodePtr node = node2->Optimize(info)) node2 = node;

	// TODO: Optimize

	return nullptr;
}

StringBuilder LogicNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node1->ToMelon(indent);
	sb += " ";
	sb += GetOperator().ToString();
	sb += " ";
	sb += node2->ToMelon(indent);
	return sb;
}