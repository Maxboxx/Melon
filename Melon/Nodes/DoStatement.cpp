#include "DoStatement.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "BreakStatement.h"
#include "EmptyStatement.h"
#include "Statements.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DoStatement::DoStatement(Symbols::Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

DoStatement::~DoStatement() {

}

bool DoStatement::IsScope() const {
	return true;
}

Ptr<Kiwi::Value> DoStatement::Compile(CompileInfo& info) {
	LoopInfo scope = LoopInfo(info.NewLabel());
	info.PushScope(scope);
	statements->Compile(info);
	info.PopScope();

	info.NewInstructionBlock(scope.endLabel);
	return nullptr;
}

void DoStatement::IncludeScan(ParsingInfo& info) {
	statements->IncludeScan(info);
}

ScanResult DoStatement::Scan(ScanInfoStack& info) {
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	ScanResult result = statements->Scan(info);
	info->scopeInfo.ExitScope();
	return result;
}

NameList DoStatement::FindSideEffectScope(const bool assign) {
	return statements->GetSideEffectScope(assign);
}

Ptr<Statement> DoStatement::Optimize(OptimizeInfo& info) {
	Node::Optimize(statements, info);

	if (IsEmpty(statements) || !statements->HasSideEffects()) {
		info.optimized = true;
		return new EmptyStatement();
	}

	return nullptr;
}

StringBuilder DoStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "do\n";
	sb += String('\t').Repeat(indent + 1);
	sb += statements->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}