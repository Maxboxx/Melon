#include "Statements.h"

#include "LoopStatement.h";
#include "MatchStatement.h";
#include "RepeatStatement.h";
#include "GuardStatement.h";
#include "FunctionBody.h";
#include "StructStatement.h";
#include "EnumStatement.h";
#include "DoStatement.h";
#include "EmptyStatement.h";

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Statements::Statements(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

Statements::~Statements() {
	
}

void Statements::IncludeScan(ParsingInfo& info) {
	for (Weak<Statement> statement : statements) {
		statement->IncludeScan(info);
	}
}

ScanResult Statements::Scan(ScanInfoStack& info) {
	ScanResult result;

	for (Weak<Statement> statement : statements) {
		ScanResult r = statement->Scan(info);
		r.SelfUseCheck(info, statement->File());
		result |= r;
	}

	return result;
}

Ptr<Statements> Statements::Optimize(OptimizeInfo& info) {
	if (statements.IsEmpty()) return nullptr;

	if (!HasSideEffects()) {
		info.optimized = true;
		statements.Clear();
		return nullptr;
	}

	for (Ptr<Statement>& statement : statements) {
		Node::Optimize(statement, info);
	}

	for (UInt i = 0; i < statements.Count(); i++) {
		if (IsEmpty(statements[i])) {
			statements.RemoveAt(i);
			i--;
			info.optimized = true;
		}
	}

	if (statements.IsEmpty()) {
		info.optimized = true;
	}

	return nullptr;
}

Ptr<Kiwi::Value> Statements::Compile(CompileInfo& info) {
	for (Weak<Statement> statement : statements) {
		statement->Compile(info);
	}
	
	return nullptr;
}

NameList Statements::FindSideEffectScope(const bool assign) {
	if (statements.IsEmpty()) return scope->AbsoluteName();

	NameList list = statements[0]->GetSideEffectScope(assign);

	for (UInt i = 1; i < statements.Count(); i++) {
		list = CombineSideEffects(list, statements[i]->GetSideEffectScope(assign));
	}

	return list;
}

StringBuilder Statements::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs = String('\t').Repeat(indent);

	bool prevSpace = false;

	for (UInt i = 0; i < statements.Count(); i++) {
		bool hasSpace = Statements::HasSpaceAround(statements[i]);

		if (i > 0) {
			if (hasSpace || prevSpace) sb += "\n";
			sb += "\n";
			sb += tabs;
		}

		sb += statements[i]->ToMelon(indent);
		prevSpace = hasSpace;
	}

	return sb;
}

bool Statements::HasSpaceAround(Weak<Statement> statement) {
	if (statement.Is<LoopStatement>())     return true;
	if (statement.Is<MatchStatement>())   return true;
	if (statement.Is<DoStatement>())       return true;
	if (statement.Is<RepeatStatement>())   return true;
	if (statement.Is<FunctionBody>()) return true;
	if (statement.Is<StructStatement>())   return true;
	if (statement.Is<EnumStatement>())     return true;

	if (Weak<GuardStatement> guardNode = statement.As<GuardStatement>()) {
		return (bool)guardNode->else_;
	}

	if (Weak<EmptyStatement> empty = statement.As<EmptyStatement>()) {
		return HasSpaceAround(empty->statement);
	}

	return false;
}

Ptr<Statements> Statements::FromStatement(Ptr<Statement> statement) {
	Ptr<Statements> statements = new Statements(statement->scope, statement->File());
	statements->statements.Add(statement);
	return statements;
}
