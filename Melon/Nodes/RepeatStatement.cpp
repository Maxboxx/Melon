#include "RepeatStatement.h"

#include "BreakStatement.h"
#include "ContinueStatement.h"
#include "DoStatement.h"
#include "EmptyStatement.h"
#include "Statements.h"
#include "Condition.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RepeatStatement::RepeatStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

RepeatStatement::~RepeatStatement() {

}

bool RepeatStatement::IsScope() const {
	return true;
}

Ptr<Kiwi::Value> RepeatStatement::Compile(CompileInfo& info) {
	const String topLbl = info.NewLabel();

	LoopInfo loop = LoopInfo(info.NewLabel(), info.NewLabel());

	info.PushLoop(loop);

	info.NewInstructionBlock(topLbl);
	statements->Compile(info);
	info.NewInstructionBlock(loop.continueLabel);

	Ptr<Kiwi::Value> value = condition->Compile(info);
	info.AddInstruction(new Kiwi::IfInstruction(value, nullptr, topLbl));

	info.NewInstructionBlock(loop.endLabel);
	info.PopLoop();

	return nullptr;
}

void RepeatStatement::IncludeScan(ParsingInfo& info) {
	statements->IncludeScan(info);
	condition->IncludeScan(info);
}

ScanResult RepeatStatement::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Loop);

	ScanResult result1 = statements->Scan(info);
	result1.SelfUseCheck(info, statements->File());

	ScanResult result2 = condition->Scan(info);
	result2.SelfUseCheck(info, condition->File());

	info->scopeInfo.ExitScope();
	info->scopeInfo = ScopeInfo::WeakBranchUnion(scopeInfo, info->scopeInfo);

	return result1 | result2;
}

NameList RepeatStatement::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(statements->GetSideEffectScope(assign), condition->GetSideEffectScope(assign));
}

Ptr<Statement> RepeatStatement::Optimize(OptimizeInfo& info) {
	Node::Optimize(statements, info);
	Node::Optimize(condition, info);
	return nullptr;
}

StringBuilder RepeatStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "repeat\n";
	sb += String('\t').Repeat(indent + 1);
	sb += statements->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "until ";
	sb += condition->ToMelon(indent);
	return sb;
}