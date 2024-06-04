#include "NewExpression.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Boolean.h"
#include "KiwiVariable.h"
#include "TypeConversion.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

NewExpression::NewExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {
	
}

NewExpression::~NewExpression() {

}

TypeSymbol* NewExpression::Type(TypeSymbol* expected) const {
	TypeSymbol* expectedInner = ExpectedInnerType(expected);
	TypeSymbol* type = operand->Type(expectedInner);

	if (type == nullptr) return nullptr;

	if (type != expectedInner && SymbolTable::FindImplicitConversion(type, expectedInner, File(), false)) {
		type = expectedInner;
	}

	Name ptrName = Name::Pointer;
	ptrName.types = List<NameList>();
	ptrName.types->Add(type->AbsoluteName());

	return SymbolTable::FindAbsolute<TypeSymbol>(NameList(ptrName), File());
}

TypeSymbol* NewExpression::ExpectedInnerType(TypeSymbol* expectedOuter) const {
	if (PtrSymbol* const ptr = expectedOuter->Cast<PtrSymbol>()) {
		return ptr->PtrType();
	}

	return nullptr;
}

Symbol* NewExpression::Symbol(TypeSymbol* expected) const {
	return nullptr;
}

Ptr<Kiwi::Value> NewExpression::Compile(CompileInfo& info) {
	TypeSymbol* const expectedInner = ExpectedInnerType(info.PeekExpectedType());
	info.PushExpectedType(expectedInner);

	TypeSymbol* type = operand->Type(expectedInner);

	Ptr<TypeConversion> convert = nullptr;

	if (type != expectedInner && SymbolTable::FindImplicitConversion(type, expectedInner, File(), false)) {
		type = expectedInner;

		Ptr<WeakExpression> weak = new WeakExpression(operand);

		convert = new TypeConversion(scope, file);
		convert->expression = weak;
		convert->isExplicit = false;
		convert->type = expectedInner->AbsoluteName();
	}

	Kiwi::Type kiwiType = type->KiwiType();
	Kiwi::Type ptrType = kiwiType;
	ptrType.pointers++;

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(ptrType, var->Copy(), new Kiwi::AllocExpression(kiwiType)));

	Ptr<KiwiVariable> v = new KiwiVariable(new Kiwi::DerefVariable(var->name), type->AbsoluteName());

	if (convert) {
		Node::CompileAssignment(v, convert, info, File(), false);
	}
	else {
		Node::CompileAssignment(v, operand, info, File(), false);
	}

	info.PopExpectedType();
	return var;
}

void NewExpression::IncludeScan(ParsingInfo& info) {
	operand->IncludeScan(info);
}

ScanResult NewExpression::Scan(ScanInfoStack& info) {
	info.PushExpectedType(ExpectedInnerType(info.PeekExpectedType()));

	ScanResult result = operand->Scan(info);
	result.SelfUseCheck(info, operand->File());

	info.PopExpectedType();

	return result;
}

NameList NewExpression::FindSideEffectScope(const bool assign) {
	return operand->GetSideEffectScope(assign);
}

Ptr<Expression> NewExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand, info);
	return nullptr;
}

StringBuilder NewExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = "new ";
	sb += operand->ToMelon(indent);
	return sb;
}
