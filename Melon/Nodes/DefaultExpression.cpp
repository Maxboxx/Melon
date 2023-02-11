#include "DefaultExpression.h"

#include "TypeExpression.h"
#include "KiwiVariable.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DefaultExpression::DefaultExpression(Symbols::Symbol* const scope, const FileInfo& file) : BinaryOperatorExpression(scope, Name::Default, file) {

}

DefaultExpression::~DefaultExpression() {

}

TypeSymbol* DefaultExpression::Type() const {
	TypeSymbol* const type1 = operand1->Type();

	// TODO: error?
	if (type1 == nullptr) return nullptr;

	Symbols::Symbol* const value = type1->Contains(Name::Value);

	// TODO: error?
	if (value == nullptr) return nullptr;

	TypeSymbol* const type = value->Type();

	// TODO: error?
	if (type == nullptr) return nullptr;

	if (!SymbolTable::FindImplicitConversion(operand2->Type(), type, file)) {
		ErrorLog::Error(LogMessage("error.type.default"), file);
	}

	return type;
}

Ptr<Kiwi::Value> DefaultExpression::Compile(CompileInfo& info) {
	Ptr<Kiwi::Variable> optional = operand1->Compile(info).AsPtr<Kiwi::Variable>();
	if (!optional) return nullptr;

	Ptr<Kiwi::Variable> var  = new Kiwi::Variable(info.NewRegister());
	info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), var->Copy(), nullptr));

	// Condition
	Ptr<Kiwi::Expression> cond = new Kiwi::SubVariable(optional->Copy(), operand1->Type()->Find(Name::HasValue, operand1->File())->KiwiName());
	const String falseLbl = info.NewLabel();
	const String endLbl   = info.NewLabel();

	info.currentBlock->AddInstruction(new Kiwi::IfInstruction(cond, nullptr, falseLbl));

	// Unwrap
	info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(var->Copy(), new Kiwi::SubVariable(optional->Copy(), operand1->Type()->Find(Name::Value, operand1->File())->KiwiName())));
	info.currentBlock->AddInstruction(new Kiwi::GotoInstruction(endLbl));

	// Default value
	info.NewInstructionBlock(falseLbl);
	Ptr<KiwiVariable> target = new KiwiVariable(var->Copy(), Type()->AbsoluteName());
	Node::CompileAssignment(target, operand2, info, operand2->File(), false);

	// End
	info.NewInstructionBlock(endLbl);
	return var;
}

void DefaultExpression::IncludeScan(ParsingInfo& info) {
	operand1->IncludeScan(info);
	operand2->IncludeScan(info);
}

ScanResult DefaultExpression::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	TypeSymbol* const type = Type();
	
	if (type) {
		Ptr<TypeExpression> tn = new TypeExpression(type->AbsoluteName());
		ScanAssignment(tn, operand2, info, file);
	}

	info->scopeInfo = scopeInfo;

	return result1 | result2;
}

NameList DefaultExpression::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
}

Ptr<Expression> DefaultExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);
	return nullptr;
}

StringBuilder DefaultExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ?? ";
	sb += operand2->ToMelon(indent);
	return sb;
}