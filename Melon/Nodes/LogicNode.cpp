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

CompiledNode LogicNode::CompileAndOrOperand(CompileInfo& info, CompiledNode& cn, List<UInt>& jumps, const bool checkTrue) const {
	// Compile operand
	CompiledNode operand = CompileToBool(node1, info);
	cn.AddInstructions(operand.instructions);

	// Compare operand with false
	Instruction comp = Instruction(checkTrue ? InstructionType::Ne : InstructionType::Eq, 1);
	comp.arguments.Add(operand.argument);
	comp.arguments.Add(Argument(0));

	// Jump if next operand is not needed
	jumps.Add(cn.instructions.Size());
	cn.instructions.Add(comp);

	// Free up register from operand
	if (operand.argument.type == ArgumentType::Register && operand.argument.reg.type == RegisterType::Register) {
		info.index--;
	}

	return operand;
}

CompiledNode LogicNode::CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) const {
	CompiledNode cn;
	cn.size = 1;

	List<UInt> jumps;

	// Compile operand 1
	CompiledNode operand1 = CompileAndOrOperand(info, cn, jumps, checkTrue);

	// Compile operand 2
	CompiledNode operand2 = CompileAndOrOperand(info, cn, jumps, checkTrue);

	cn.argument = Argument(Register(info.index++));

	// Set result if all operand comps failed
	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(cn.argument);
	mov1.arguments.Add(Argument(setTrue ? 0 : 1));
	cn.instructions.Add(mov1);

	// Jump to end
	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, (ULong)info.label + 1));
	cn.instructions.Add(jmp);

	// Add label and insert jumps
	cn.instructions.Add(Instruction::Label(info.label));

	for (const UInt jump : jumps) {
		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	info.label++;

	// Set result if an operand comp was successful
	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(cn.argument);
	mov2.arguments.Add(Argument(setTrue ? 1 : 0));
	cn.instructions.Add(mov2);

	// Add end label
	cn.instructions.Add(Instruction::Label(info.label++));

	return cn;
}

CompiledNode LogicNode::CompileXor(CompileInfo& info, const bool checkEqual) const {
	CompiledNode cn;
	cn.argument = Argument(Register(info.index++));
	cn.size = 1;

	// Compile operand 1
	CompiledNode operand1 = CompileToBool(node1, info);
	cn.AddInstructions(operand1.instructions);

	// Compile operand 2
	CompiledNode operand2 = CompileToBool(node2, info);
	cn.AddInstructions(operand2.instructions);

	// Check if operand results are equal or not
	Instruction comp = Instruction(checkEqual ? InstructionType::Eq : InstructionType::Ne, 1);
	comp.arguments.Add(operand1.argument);
	comp.arguments.Add(operand2.argument);
	comp.arguments.Add(cn.argument);
	cn.instructions.Add(comp);

	// Free up registers
	if (operand2.argument.type == ArgumentType::Register && operand2.argument.reg.type == RegisterType::Register) {
		info.index--;

		if (operand1.argument.type == ArgumentType::Register && operand1.argument.reg.type == RegisterType::Register) {
			info.index--;
		}
	}

	return cn;
}

CompiledNode LogicNode::Compile(CompileInfo& info) {
	switch (type) {
		case TokenType::Or:   return CompileAndOr(info, true, true);
		case TokenType::And:  return CompileAndOr(info, false, false);
		case TokenType::Xor:  return CompileXor(info, false);
		case TokenType::Nor:  return CompileAndOr(info, true, false);
		case TokenType::Nand: return CompileAndOr(info, false, true);
		case TokenType::Xnor: return CompileXor(info, true);
	}

	return CompiledNode();
}

ScanResult LogicNode::Scan(ScanInfoStack& info) {
	// Scan operands
	ScanResult result1 = node1->Scan(info);
	result1.SelfUseCheck(info, node1->file);

	ScanResult result2 = node2->Scan(info);
	result2.SelfUseCheck(info, node2->file);

	// Scan conversion to bool for both operands
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
