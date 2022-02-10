#include "Statements.h"

#include "LoopStatement.h";
#include "SwitchStatement.h";
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

UInt Statements::GetSize() const {
	UInt size = 0;

	for (UInt i = statements.Size(); i > 0;) {
		i--;

		if (statements[i]->IsScope()) {
			size = Math::Max(size, statements[i]->GetSize());
		}
		else {
			size += statements[i]->GetSize();
		}
	}

	return size;
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

	for (UInt i = 0; i < statements.Size(); i++) {
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

CompiledNode Statements::Compile(CompileInfo& info) {
	CompiledNode c;

	for (Weak<Statement> statement : statements) {
		info.index = 0;
		c.AddInstructions(statement->Compile(info).instructions);
	}

	info.index = 0;

	return c;
}

NameList Statements::FindSideEffectScope(const bool assign) {
	if (statements.IsEmpty()) return scope->AbsoluteName();

	NameList list = statements[0]->GetSideEffectScope(assign);

	for (UInt i = 1; i < statements.Size(); i++) {
		list = CombineSideEffects(list, statements[i]->GetSideEffectScope(assign));
	}

	return list;
}

StringBuilder Statements::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs = String('\t').Repeat(indent);

	bool prevSpace = false;

	for (UInt i = 0; i < statements.Size(); i++) {
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
	if (statement.Is<SwitchStatement>())   return true;
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
