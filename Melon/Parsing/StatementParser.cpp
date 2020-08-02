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

#include "Melon/Nodes/StatementsNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NodePtr StatementParser::Parse(ParsingInfo& info) {
	if (NodePtr node = CallStatementParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = AssignmentParser::Parse(info)) {
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

	return nullptr;
}

NodePtr StatementParser::ParseMultiple(ParsingInfo& info) {
	Pointer<StatementsNode> sn = new StatementsNode(info.scopes, FileInfo(info.filename, info.Current().line));

	while (NodePtr stat = StatementParser::Parse(info))
		sn->statements.Add(stat);

	return sn;
}