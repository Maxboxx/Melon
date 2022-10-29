#include "UnaryOperatorExpression.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Boolean.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

UnaryOperatorExpression::UnaryOperatorExpression(Symbols::Symbol* const scope, const Name& op, const FileInfo& file) : Expression(scope, file) {
	this->op = op;
}

UnaryOperatorExpression::~UnaryOperatorExpression() {

}

TypeSymbol* UnaryOperatorExpression::Type() const {
	List<TypeSymbol*> args;
	args.Add(operand->Type());

	TypeSymbol* const type = operand->Type();

	FunctionSymbol* const s = type->FindUnaryOperator(op, file);

	if (s && !s->returnValues.IsEmpty()) {
		TypeSymbol* const s2 = s->ReturnType(0);

		if (s2 && s2->Is<TemplateSymbol>()) {
			return s2->Type();
		}
		
		return s2;
	}

	return nullptr;
}

Symbol* UnaryOperatorExpression::Symbol() const {
	if (op == Name::Unwrap) {
		if (TypeSymbol* const type = operand->Type()) {
			return type->Find<VariableSymbol>(Name::Value, file);
		}
	}

	return nullptr;
}

Name UnaryOperatorExpression::GetOperator() const {
	return op;
}

CompiledNode UnaryOperatorExpression::Compile(OldCompileInfo& info) {
	TypeSymbol* const type = operand->Type();

	FunctionSymbol* const func = type->FindUnaryOperator(op, file);

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(operand, info);
	}
	// Compile operator function
	else {
		List<Ptr<Expression>> args;
		args.Add(new WeakExpression(operand));

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->expression = new TypeExpression(func->ParentType()->AbsoluteName());

		return cn->Compile(info);
	}
}


Ptr<Kiwi::Value> UnaryOperatorExpression::Compile(CompileInfo& info) {
	TypeSymbol* const type = operand->Type();

	FunctionSymbol* const func = type->FindUnaryOperator(op, file);
	if (!func) return nullptr;

	// Compile symbol node
	if (func->symbolNode) {
		//return func->symbolNode->Compile(operand, info);
		Ptr<Kiwi::Expression> expression;

		if (op.name == Name::Neg.name) {
			expression = new Kiwi::NegExpression(operand->Compile(info));
		}
		else if (op.name == Name::BitNot.name) {
			expression = new Kiwi::BitNotExpression(operand->Compile(info));
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
		args.Add(new WeakExpression(operand));

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->expression = new TypeExpression(func->ParentType()->AbsoluteName());

		//return cn->Compile(info);
		return nullptr;
	}
}

void UnaryOperatorExpression::IncludeScan(ParsingInfo& info) {
	operand->IncludeScan(info);
}

ScanResult UnaryOperatorExpression::Scan(ScanInfoStack& info) {
	ScanResult result = operand->Scan(info);
	result.SelfUseCheck(info, operand->File());

	if (op == Name::Unwrap) {
		// TODO: fix
		ErrorLog::Warning(LogMessage::Message("unwrap operator does not work properly for nil values"), file);
	}

	if (TypeSymbol* const type = operand->Type()) {
		type->FindUnaryOperator(op, file);
	}

	return result;
}

NameList UnaryOperatorExpression::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return operand->GetSideEffectScope(assign);
}

Ptr<Expression> UnaryOperatorExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand, info);

	// TODO: Add more operators
	if (operand->IsImmediate()) {
		if (op == Name::Not) {
			Ptr<Boolean> bn = new Boolean(operand->File());
			bn->value = operand->GetImmediate() == 0;
			info.optimized = true;
			return bn;
		}
	}

	return nullptr;
}

StringBuilder UnaryOperatorExpression::ToMelon(const UInt indent) const {
	if (op == Name::Unwrap) {
		StringBuilder sb = operand->ToMelon(indent);
		sb += op.ToString();
		return sb;
	}
	else {
		StringBuilder sb = op.ToString();

		if (op == Name::Not) sb += " ";

		sb += operand->ToMelon(indent);
		return sb;
	}
}