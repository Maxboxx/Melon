#include "IndexExpression.h"

#include "Boxx/Math.h"

#include "CallExpression.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

IndexExpression::IndexExpression(Symbols::Symbol* scope, const FileInfo& file) : Expression(scope, file) {

}

IndexExpression::~IndexExpression() {

}

FunctionSymbol* IndexExpression::GetFunc() const {
	List<TypeSymbol*> argTypes;
	
	for (const Ptr<Expression>& arg : args) {
		argTypes.Add(arg->Type());
	}

	TypeSymbol* type = expression->Type();
	return type->FindMethod(Name::Index, argTypes, File());
}

FunctionSymbol* IndexExpression::AssignFunc(TypeSymbol* type) const {
	List<TypeSymbol*> argTypes;

	for (const Ptr<Expression>& arg : args) {
		argTypes.Add(arg->Type());
	}

	argTypes.Add(type);

	TypeSymbol* exprType = expression->Type();
	return exprType->FindMethod(Name::Index, argTypes, File());
}

Ptr<Kiwi::Value> IndexExpression::CompileAssignFunc(FunctionSymbol* func, Weak<Expression> expr, CompileInfo& info) const {
	if (!func->symbolNode) return nullptr;

	List<Weak<Expression>> args;
	args.Add(expression);
	args.Add(this->args[0]);
	args.Add(expr);

	return func->symbolNode->Compile(args, info, false);
}

Symbol* IndexExpression::Symbol() const {
	return Type();
}

TypeSymbol* IndexExpression::Type(TypeSymbol* expected) const {
	FunctionSymbol* func = GetFunc();

	if (!func) return nullptr;

	return func->ReturnType(0);
}

Ptr<Kiwi::Value> IndexExpression::Compile(CompileInfo& info) {
	FunctionSymbol* const func = GetFunc();
	if (!func) return nullptr;

	// Compile symbol node
	if (func->symbolNode) {
		List<Weak<Expression>> operands;
		operands.Add(expression);

		for (UInt i = 0; i < args.Count(); i++) {
			operands.Add(args[i]);
		}

		return func->symbolNode->Compile(operands, info, false);
	}
	// Compile operator function
	else {
		List<Ptr<Expression>> operands;
		operands.Add(new WeakExpression(expression));

		for (UInt i = 0; i < args.Count(); i++) {
			operands.Add(new WeakExpression(args[i]));
		}

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->operatorFunction = func;

		return cn->Compile(info);
	}
}

StringBuilder IndexExpression::ToMelon(const UInt indent) const {
	StringBuilder builder = expression->ToMelon(indent);

	builder += '[';

	for (UInt i = 0; i < args.Count(); i++) {
		if (i > 0) builder += ", ";
		builder += args[i]->ToMelon(indent);
	}

	builder += ']';

	return builder;
}