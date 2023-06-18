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

TypeSymbol* LogicExpression::Type(TypeSymbol* expected) const {
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

Ptr<Kiwi::Value> LogicExpression::CompileToBool(Weak<Expression> node, CompileInfo& info) {
	Ptr<TypeConversion> convert = new TypeConversion(node->scope, node->File());
	convert->expression = new WeakExpression(node);
	convert->type = NameList::Bool;
	convert->isExplicit = true;
	return convert->Compile(info);
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
