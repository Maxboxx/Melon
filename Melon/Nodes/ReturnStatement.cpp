#include "ReturnStatement.h"

#include "KiwiMemoryExpression.h"
#include "TypeExpression.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

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
		for (UInt i = 0; i < f->returnValues.Size(); i++) {
			types.Add(f->ReturnType(i));
		}
	}

	return types;
}

CompiledNode ReturnStatement::Compile(CompileInfo& info) {
	FunctionSymbol* const f = GetFunc();
	if (!f) return CompiledNode();

	UInt stackOffset  = info.stack.ptrSize + info.stack.frame;
	UInt argumentSize = f->ArgumentSize();
	UInt returnSize   = f->ReturnSize();
	
	stackOffset += argumentSize;
	stackOffset += returnSize;

	CompiledNode c;
	List<TypeSymbol*> types = GetTypes();

	// Compile return values
	for (UInt i = 0; i < values.Size(); i++) {
		stackOffset -= types[i]->Size();

		Ptr<KiwiMemoryExpression> sn = new KiwiMemoryExpression(stackOffset, types[i]->AbsoluteName());

		info.important = true;
		c.AddInstructions(CompileAssignment(sn, values[i], info, values[i]->File()).instructions);
		info.important = false;
	}

	// Add return instruction
	info.stack.PopExpr(0, c);
	c.instructions.Add(Instruction(InstructionType::Ret));

	return c;
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
	if (f->returnValues.Size() > values.Size()) {
		ErrorLog::Error(LogMessage("error.scan.return.many"), file);
	}
	else if (f->returnValues.Size() < values.Size()) {
		ErrorLog::Error(LogMessage("error.scan.return.few"), file);
	}

	List<TypeSymbol*> types = GetTypes();

	// Scan assignment to return values
	for (UInt i = 0; i < values.Size(); i++) {
		if (i >= types.Size()) break;

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

	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		else sb += " ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}