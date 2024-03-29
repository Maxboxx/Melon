#include "ReturnStatement.h"

#include "KiwiVariable.h"
#include "TypeExpression.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ReturnStatement::ReturnStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

ReturnStatement::~ReturnStatement() {

}

FunctionSymbol* ReturnStatement::GetFunc() const {
	return SymbolTable::Find<FunctionSymbol>(func, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

List<TypeSymbol*> ReturnStatement::GetTypes() const {
	List<TypeSymbol*> types;

	if (FunctionSymbol* const f = GetFunc()) {
		for (UInt i = 0; i < f->returnValues.Count(); i++) {
			types.Add(f->ReturnType(i));
		}
	}

	return types;
}

Ptr<Kiwi::Value> ReturnStatement::Compile(CompileInfo& info) {
	FunctionSymbol* const f = GetFunc();
	if (!f) return nullptr;

	List<TypeSymbol*> types = GetTypes();

	for (UInt i = 0; i < values.Count(); i++) {
		Ptr<KiwiVariable> reg = new KiwiVariable(new Kiwi::Variable(info.returnRegisters[i]), f->ReturnType(i)->AbsoluteName());
		CompileAssignment(reg, values[i], info, values[i]->File(), false);
	}

	info.currentBlock->AddInstruction(new Kiwi::ReturnInstruction());

	return nullptr;
}

void ReturnStatement::IncludeScan(ParsingInfo& info) {
	for (Weak<Expression> value : values) {
		value->IncludeScan(info);
	}
}

ScanResult ReturnStatement::Scan(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		info->scopeInfo.hasReturned = true;
	}

	info->scopeInfo.willNotReturn = false;

	ScanResult result;

	// Scan nodes
	for (Weak<Expression> value : values) {
		ScanResult r = value->Scan(info);
		r.SelfUseCheck(info, value->File());
		result |= r;
	}

	FunctionSymbol* const f = GetFunc();

	if (f == nullptr) return result;

	// Check for correct number of return values
	if (f->returnValues.Count() > values.Count()) {
		ErrorLog::Error(LogMessage("error.scan.return.many"), file);
	}
	else if (f->returnValues.Count() < values.Count()) {
		ErrorLog::Error(LogMessage("error.scan.return.few"), file);
	}

	List<TypeSymbol*> types = GetTypes();

	// Scan assignment to return values
	for (UInt i = 0; i < values.Count(); i++) {
		if (i >= types.Count()) break;

		if (types[i]) {
			Ptr<TypeExpression> tn = new TypeExpression(types[i]->AbsoluteName());
			ScanAssignment(tn, values[i], info, values[i]->File());
		}
	}

	return result;
}

NameList ReturnStatement::FindSideEffectScope(const bool assign) {
	NameList list = values.IsEmpty() ? scope->AbsoluteName() : scope->Parent()->AbsoluteName();

	for (Weak<Expression> value : values) {
		list = CombineSideEffects(list, value->GetSideEffectScope(assign));
	}

	return list;
}

Ptr<Statement> ReturnStatement::Optimize(OptimizeInfo& info) {
	for (Ptr<Expression>& value : values) {
		Node::Optimize(value, info);
	}

	return nullptr;
}

StringBuilder ReturnStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "return";

	for (UInt i = 0; i < values.Count(); i++) {
		if (i > 0) sb += ", ";
		else sb += " ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}