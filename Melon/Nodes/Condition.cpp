#include "Condition.h"

#include "TypeConversion.h"
#include "Assignment.h"
#include "Boolean.h"
#include "TypeExpression.h"
#include "DiscardExpression.h"
#include "KiwiVariable.h"

#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

Condition::Condition(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

Condition::~Condition() {
	
}


TypeSymbol* Condition::Type() const {
	return assign ? (TypeSymbol*)SymbolTable::Bool : expression->Type();
}

Ptr<Kiwi::Value> Condition::Compile(CompileInfo& info) {
	if (assign) {
		TypeSymbol* const optionalType = assign->values[0]->Type();

		Ptr<Kiwi::Variable> optional = assign->values[0]->Compile(info).AsPtr<Kiwi::Variable>();
		Ptr<Kiwi::SubVariable> hasValue = new Kiwi::SubVariable(optional->Copy(), optionalType->Find(Name::HasValue, assign->values[0]->File())->KiwiName());
		Ptr<Kiwi::Variable> var = new Kiwi::Variable(assign->assignableValues[0]->Symbol()->KiwiName());

		const String lbl = info.NewLabel();

		info.AddInstruction(new Kiwi::IfInstruction(hasValue->Copy(), nullptr, lbl));

		Ptr<KiwiVariable> kiwiVar1 = new KiwiVariable(var->Copy(), assign->assignableValues[0]->Type()->AbsoluteName());
		Ptr<KiwiVariable> kiwiVar2 = new KiwiVariable(new Kiwi::SubVariable(optional->Copy(), optionalType->Find(Name::Value, assign->values[0]->File())->KiwiName()), optionalType->Find(Name::Value, assign->values[0]->File())->Type()->AbsoluteName());
		CompileAssignment(kiwiVar1, kiwiVar2, info, File(), true);

		info.NewInstructionBlock(lbl);
		return hasValue;
	}
	else {
		Ptr<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = new WeakExpression(expression);
		convert->type = NameList::Bool;
		return convert->Compile(info);
	}
}

void Condition::IncludeScan(ParsingInfo& info) {
	if (assign) {
		assign->IncludeScan(info);
	}
	else {
		expression->IncludeScan(info);
	}
}

ScanResult Condition::Scan(ScanInfoStack& info) {
	// Scan assignment condition
	if (assign) {
		Ptr<Expression> tempValue = assign->values[0];
		
		TypeSymbol* const type = tempValue->Type();

		// Check if the types match
		if (type && type->AbsoluteName()[0].name == Name::Optional.name) {
			assign->values[0] = new TypeExpression(type->Find(Name::Value, file)->Type()->AbsoluteName());
		}
		else {
			ErrorLog::Error(LogMessage("error.type.conditional_assign", type->ToString()), tempValue->File());
		}

		ScanResult result = assign->Scan(info);
		assign->values[0] = tempValue;
		return result;
	}
	// Scan regular condition
	else {
		Ptr<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = new WeakExpression(expression);
		convert->type = NameList::Bool;
		return convert->Scan(info);
	}
}

NameList Condition::FindSideEffectScope(const bool assign) {
	return expression->GetSideEffectScope(assign);
}

Ptr<Condition> Condition::Optimize(OptimizeInfo& info) {
	// Optimize assignment
	if (assign) {
		assign->OptimizeAsCondition(info);

		if (assign->assignableValues.IsEmpty() || assign->assignableValues[0].Is<DiscardExpression>()) {
			expression = assign->values[0];
			assign = nullptr;
		}
	}
	// Optimize regular condition
	else {
		Node::Optimize(expression, info);
	}

	return nullptr;
}

StringBuilder Condition::ToMelon(const UInt indent) const {
	if (assign) {
		return assign->ToMelon(indent);
	}
	else {
		return expression->ToMelon(indent);
	}
}

Ptr<Condition> Condition::FromExpression(Ptr<Expression> expression) {
	Ptr<Condition> cond = new Condition(expression->scope, expression->File());
	cond->expression = expression;
	return cond;
}
