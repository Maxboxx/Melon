#include "LogicExpression.h"

#include "TypeConversion.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Symbols::Nodes;

LogicExpression::LogicExpression(Symbols::Symbol* const scope, const TokenType type, const FileInfo& file) : BinaryOperatorExpression(scope, Name(), file) {
	this->type = type;
}

LogicExpression::~LogicExpression() {

}

TypeSymbol* LogicExpression::Type() const {
	return (TypeSymbol*)SymbolTable::Bool;
}

Name LogicExpression::GetOperator() const {
	switch (type) {
		case TokenType::Or:   return Name::Or;
		case TokenType::And:  return Name::And;
		case TokenType::Xor:  return Name::Xor;
		case TokenType::Nor:  return Name::Nor;
		case TokenType::Nand: return Name::Nand;
		case TokenType::Xnor: return Name::Xnor;
	}

	return Name("logic");
}

CompiledNode LogicExpression::CompileToBool(Ptr<Expression>& node, OldCompileInfo& info) {
	Ptr<TypeConversion> convert = new TypeConversion(node->scope, node->File());
	convert->expression = node;
	convert->type = NameList::Bool;
	convert->isExplicit = true;

	CompiledNode cn = convert->Compile(info);
	node = convert->expression;
	return cn;
}

Ptr<Kiwi::Value> LogicExpression::CompileToBool(Weak<Expression> node, CompileInfo& info) {
	Ptr<TypeConversion> convert = new TypeConversion(node->scope, node->File());
	convert->expression = new WeakExpression(node);
	convert->type = NameList::Bool;
	convert->isExplicit = true;
	return convert->Compile(info);
}

CompiledNode LogicExpression::CompileAndOrOperand(OldCompileInfo& info, CompiledNode& cn, List<UInt>& jumps, const bool checkTrue) {
	// Compile operand
	CompiledNode operand = CompileToBool(operand1, info);
	cn.AddInstructions(operand.instructions);

	// Compare operand with false
	Instruction comp = Instruction(checkTrue ? InstructionType::Ne : InstructionType::Eq, 1);
	comp.arguments.Add(operand.argument);
	comp.arguments.Add(Argument(0));

	// Jump if next operand is not needed
	jumps.Add(cn.instructions.Count());
	cn.instructions.Add(comp);

	// Free up register from operand
	if (operand.argument.type == ArgumentType::Register && operand.argument.reg.type == RegisterType::Register) {
		info.index--;
	}

	return operand;
}

CompiledNode LogicExpression::CompileAndOr(OldCompileInfo& info, const bool checkTrue, const bool setTrue) {
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

Ptr<Kiwi::Value> LogicExpression::CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) {
	Ptr<Kiwi::Variable> result = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), result->Copy(), nullptr));

	const String nextLbl = info.NewLabel();
	const String endLbl  = info.NewLabel();

	if (checkTrue) {
		info.AddInstruction(new Kiwi::IfInstruction(CompileToBool(operand1, info), nullptr, nextLbl));
	}
	else {
		info.AddInstruction(new Kiwi::IfInstruction(CompileToBool(operand1, info), nextLbl, nullptr));
	}

	info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), new Kiwi::Integer(setTrue ? 1 : 0)));
	info.AddInstruction(new Kiwi::GotoInstruction(endLbl));

	info.NewInstructionBlock(nextLbl);

	info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), CompileToBool(operand2, info)));

	if (checkTrue ^ setTrue) {
		info.AddInstruction(new Kiwi::AssignInstruction(result->Copy(), new Kiwi::EqualExpression(result->Copy(), new Kiwi::Integer(0))));
	}

	info.NewInstructionBlock(endLbl);
	return result;
}

CompiledNode LogicExpression::CompileXor(OldCompileInfo& info, const bool checkEqual) {
	CompiledNode cn;
	cn.argument = Argument(Register(info.index++));
	cn.size = 1;

	// Compile operand 1
	CompiledNode operand1 = CompileToBool(this->operand1, info);
	cn.AddInstructions(operand1.instructions);

	// Compile operand 2
	CompiledNode operand2 = CompileToBool(this->operand2, info);
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

Ptr<Kiwi::Value> LogicExpression::CompileXor(CompileInfo& info, const bool checkEqual) {
	Ptr<Kiwi::Value> v1 = operand1->Compile(info);
	Ptr<Kiwi::Value> v2 = operand2->Compile(info);
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	Ptr<Kiwi::Expression> expr;

	if (checkEqual) {
		expr = new Kiwi::EqualExpression(v1, v2);
	}
	else {
		expr = new Kiwi::NotEqualExpression(v1, v2);
	}

	info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), var->Copy(), expr));
	return var;
}

CompiledNode LogicExpression::Compile(OldCompileInfo& info) {
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

Ptr<Kiwi::Value> LogicExpression::Compile(CompileInfo& info) {
	switch (type) {
		case TokenType::Or:   return CompileAndOr(info, true, true);
		case TokenType::And:  return CompileAndOr(info, false, false);
		case TokenType::Xor:  return CompileXor(info, false);
		case TokenType::Nor:  return CompileAndOr(info, true, false);
		case TokenType::Nand: return CompileAndOr(info, false, true);
		case TokenType::Xnor: return CompileXor(info, true);
	}

	return nullptr;
}

ScanResult LogicExpression::Scan(ScanInfoStack& info) {
	// Scan operands
	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	// Scan conversion to bool for both operands
	Pointer<TypeConversion> convert1 = new TypeConversion(operand1->scope, operand1->File());
	convert1->expression = new WeakExpression(operand1);
	convert1->type = NameList::Bool;
	convert1->isExplicit = true;

	ScanResult result3 = convert1->Scan(info);

	Pointer<TypeConversion> convert2 = new TypeConversion(operand2->scope, operand2->File());
	convert2->expression = new WeakExpression(operand2);
	convert2->type = NameList::Bool;
	convert2->isExplicit = true;

	ScanResult result4 = convert2->Scan(info);

	return result1 | result2 | result3 | result4;
}

NameList LogicExpression::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
}

Ptr<Expression> LogicExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);

	// TODO: Optimize

	return nullptr;
}

StringBuilder LogicExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ";
	sb += GetOperator().ToString();
	sb += " ";
	sb += operand2->ToMelon(indent);
	return sb;
}
