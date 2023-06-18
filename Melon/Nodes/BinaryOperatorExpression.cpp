#include "BinaryOperatorExpression.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Boolean.h"

#include "Kiwi/Expressions.h"
#include "Kiwi/Value.h"
#include "Kiwi/KiwiProgram.h"
#include "Kiwi/Interpreter/Interpreter.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BinaryOperatorExpression::BinaryOperatorExpression(Symbols::Symbol* const scope, const Name& op, const FileInfo& file) : Expression(scope, file) {
	this->op = op;
}

BinaryOperatorExpression::~BinaryOperatorExpression() {
	
}

TypeSymbol* BinaryOperatorExpression::Type(TypeSymbol* expected) const {
	FunctionSymbol* const s = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

	if (s && !s->returnValues.IsEmpty()) {
		if (TypeSymbol* const s2 = s->ReturnType(0)) {
			if (TemplateSymbol* const t = s2->Cast<TemplateSymbol>()) {
				return t->Type();
			}
			
			return s2;
		}
	}

	return nullptr;
}

Name BinaryOperatorExpression::GetOperator() const {
	return op;
}

Ptr<Kiwi::Value> BinaryOperatorExpression::Compile(CompileInfo& info) {
	FunctionSymbol* const func = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);
	if (!func) return nullptr;

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(operand1, operand2, info, false);
	}
	// Compile operator function
	else {
		List<Ptr<Expression>> args;
		args.Add(new WeakExpression(operand1));
		args.Add(new WeakExpression(operand2));

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->operatorFunction = func;

		return cn->Compile(info);
	}
}

void BinaryOperatorExpression::IncludeScan(ParsingInfo& info) {
	operand1->IncludeScan(info);
	operand2->IncludeScan(info);
}

ScanResult BinaryOperatorExpression::Scan(ScanInfoStack& info) {
	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

	return result1 | result2;
}

NameList BinaryOperatorExpression::FindSideEffectScope(const bool assign) {
	FunctionSymbol* const f = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

	if (f->symbolNode) {
		return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
	}
	else {
		// TODO: Check operator function
		return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
	}

	return NameList();
}

Ptr<Expression> BinaryOperatorExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);
	return nullptr;
}

StringBuilder BinaryOperatorExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ";
	sb += op.ToString();
	sb += " ";
	sb += operand2->ToMelon(indent);
	return sb;
}
