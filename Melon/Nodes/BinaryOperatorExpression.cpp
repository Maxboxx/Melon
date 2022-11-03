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

TypeSymbol* BinaryOperatorExpression::Type() const {
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

CompiledNode BinaryOperatorExpression::Compile(OldCompileInfo& info) {
	FunctionSymbol* const func = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);
	if (!func) return CompiledNode();

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(operand1, operand2, info);
	}
	// Compile operator function
	else {
		List<Ptr<Expression>> args;
		args.Add(operand1);
		args.Add(operand2);

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->expression = new TypeExpression(func->Parent()->Parent()->AbsoluteName());

		CompiledNode c = cn->Compile(info);
		operand1 = args[0];
		operand2 = args[1];
		return c;
	}
}

Ptr<Kiwi::Value> BinaryOperatorExpression::Compile(CompileInfo& info) {
	FunctionSymbol* const func = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);
	if (!func) return nullptr;

	// Compile symbol node
	if (func->symbolNode) {
		Ptr<Kiwi::Expression> expression;

		if (op.name == Name::Add.name) {
			expression = new Kiwi::AddExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::Sub.name) {
			expression = new Kiwi::SubExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::Mul.name) {
			expression = new Kiwi::MulExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::IDiv.name) {
			expression = new Kiwi::DivExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::BitOr.name) {
			expression = new Kiwi::BitOrExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::BitAnd.name) {
			expression = new Kiwi::BitAndExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::BitXor.name) {
			expression = new Kiwi::BitXorExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::ShiftLeft.name) {
			expression = new Kiwi::LeftShiftExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::ShiftRight.name) {
			expression = new Kiwi::RightShiftExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::Equal.name) {
			expression = new Kiwi::EqualExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::NotEqual.name) {
			expression = new Kiwi::NotEqualExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::Less.name) {
			expression = new Kiwi::LessExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::Greater.name) {
			expression = new Kiwi::GreaterExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::LessEqual.name) {
			expression = new Kiwi::LessEqualExpression(operand1->Compile(info), operand2->Compile(info));
		}
		else if (op.name == Name::GreaterEqual.name) {
			expression = new Kiwi::GreaterEqualExpression(operand1->Compile(info), operand2->Compile(info));
		}

		Ptr<Kiwi::Variable> reg = new Kiwi::Variable(info.NewRegister());

		info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(
			func->ReturnType(0)->KiwiType(), 
			reg->name, 
			expression
		));

		return reg;
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

	// TODO: Add more operators
	// Optimize immediate operands
	if (operand1->IsImmediate() && operand2->IsImmediate()) {
		// Bool operands
		if (operand1->Type()->AbsoluteName() == NameList::Bool && operand2->Type()->AbsoluteName() == NameList::Bool) {
			// Equal
			if (op == Name::Equal) {
				Ptr<Boolean> bn = new Boolean(operand1->File());
				bn->value = operand1->GetImmediate() == operand2->GetImmediate();
				info.optimized = true;
				return bn;
			}
			// Not Equal
			else if (op == Name::NotEqual) {
				Ptr<Boolean> bn = new Boolean(operand1->File());
				bn->value = operand1->GetImmediate() != operand2->GetImmediate();
				info.optimized = true;
				return bn;
			}
		}
	}

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
