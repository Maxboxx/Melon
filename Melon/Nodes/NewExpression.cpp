#include "NewExpression.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Boolean.h"
#include "KiwiVariable.h"

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
	List<TypeSymbol*> args;
	args.Add(operand->Type());

	TypeSymbol* const type = operand->Type();

	if (type == nullptr) return nullptr;

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

Symbol* NewExpression::Symbol() const {
	return nullptr;
}

Ptr<Kiwi::Value> NewExpression::Compile(CompileInfo& info) {
	TypeSymbol* const expectedInner = ExpectedInnerType(info.PeekExpectedType());
	info.PushExpectedType(expectedInner);

	TypeSymbol* const type = operand->Type(expectedInner);

	Kiwi::Type kiwiType = type->KiwiType();
	Kiwi::Type ptrType = kiwiType;
	ptrType.pointers++;

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(ptrType, var->Copy(), new Kiwi::AllocExpression(kiwiType)));

	Ptr<KiwiVariable> v = new KiwiVariable(new Kiwi::DerefVariable(var->name), type->AbsoluteName());

	Node::CompileAssignment(v, operand, info, File(), false);

	info.PopExpectedType();
	return var;
}

void NewExpression::IncludeScan(ParsingInfo& info) {
	operand->IncludeScan(info);
}

ScanResult NewExpression::Scan(ScanInfoStack& info) {
	ScanResult result = operand->Scan(info);
	result.SelfUseCheck(info, operand->File());
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
