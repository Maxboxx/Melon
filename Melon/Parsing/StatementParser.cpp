#include "StatementParser.h"

#include "CallStatementParser.h"
#include "AssignmentParser.h"
#include "LoopParser.h"
#include "SwitchParser.h"
#include "DoParser.h"
#include "FunctionParser.h"
#include "ReturnParser.h"
#include "BreakParser.h"
#include "ContinueParser.h"
#include "StructParser.h"
#include "RepeatParser.h"
#include "EnumParser.h"
#include "GuardParser.h"

#include "Melon/Nodes/StatementsNode.h"
#include "Melon/Nodes/GuardNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NodePtr StatementParser::Parse(ParsingInfo& info, const bool single) {
	if (NodePtr node = CallStatementParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = AssignmentParser::Parse(info, single)) {
		return node;
	}
	else if (NodePtr node = LoopParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = SwitchParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = DoParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = RepeatParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = GuardParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = FunctionParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = ReturnParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = BreakParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = ContinueParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = StructParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = EnumParser::Parse(info)) {
		return node;
	}

	return nullptr;
}

NodePtr StatementParser::ParseMultiple(ParsingInfo& info) {
	Pointer<StatementsNode> sn = new StatementsNode(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

	while (NodePtr stat = StatementParser::Parse(info)) {
		if (Pointer<GuardNode> gn = stat.Cast<GuardNode>()) {
			gn->continue_ = StatementParser::ParseMultiple(info);
			sn->statements.Add(gn);
		}
		else {
			sn->statements.Add(stat);
		}
	}

	return sn;
}