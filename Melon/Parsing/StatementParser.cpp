#include "StatementParser.h"

#include "CallParser.h"
#include "AssignmentParser.h"
#include "LoopParser.h"
#include "MatchParser.h"
#include "DoParser.h"
#include "FunctionParser.h"
#include "ReturnParser.h"
#include "BreakParser.h"
#include "ContinueParser.h"
#include "StructParser.h"
#include "ClassParser.h"
#include "RepeatParser.h"
#include "EnumParser.h"
#include "GuardParser.h"
#include "DebugParser.h"

#include "Melon/Nodes/GuardStatement.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

Ptr<Statement> StatementParser::Parse(ParsingInfo& info, const bool single) {
	if (info.EndOfFile()) return nullptr;

	if (Ptr<Statement> node = DebugParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = CallParser::ParseStatement(info)) {
		return node;
	}
	else if (Ptr<Statement> node = AssignmentParser::Parse(info, single ? AssignmentParser::Flags::Single : AssignmentParser::Flags::None)) {
		return node;
	}
	else if (Ptr<Statement> node = LoopParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = MatchParser::ParseStatement(info)) {
		return node;
	}
	else if (Ptr<Statement> node = DoParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = RepeatParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = GuardParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = FunctionParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = ReturnParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = BreakParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = ContinueParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = StructParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = ClassParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Statement> node = EnumParser::Parse(info)) {
		return node;
	}

	return nullptr;
}

Ptr<Statements> StatementParser::ParseMultiple(ParsingInfo& info) {
	Ptr<Statements> sn = new Statements(info.scope, info.GetFileInfo());

	while (Ptr<Statement> stat = StatementParser::Parse(info)) {
		if (Ptr<GuardStatement> gn = stat.AsPtr<GuardStatement>()) {
			gn->continue_ = StatementParser::ParseMultiple(info);
			sn->statements.Add(gn);
		}
		else {
			sn->statements.Add(stat);
		}
	}

	return sn;
}