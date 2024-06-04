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
	return type->FindMethod(Name::Index, argTypes, File(), isAssignable ? FunctionAttributes::Set : FunctionAttributes::None);
}

FunctionSymbol* IndexExpression::AssignFunc(TypeSymbol* type) const {
	FunctionSymbol* sym = GetFunc();
	if (!sym) return nullptr;
	if (sym->attributes != FunctionAttributes::Set) return nullptr;
	return sym;
}

Ptr<Kiwi::Value> IndexExpression::CompileAssignFunc(FunctionSymbol* func, Weak<Expression> expr, CompileInfo& info) const {
	if (!func->symbolNode) return nullptr;

	List<Weak<Expression>> args;
	args.Add(expression);

	for (Weak<Expression> arg : this->args) {
		args.Add(arg);
	}

	args.Add(expr);

	return func->symbolNode->Compile(args, info, false);
}

Symbol* IndexExpression::Symbol(TypeSymbol* expected) const {
	return Type(expected);
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