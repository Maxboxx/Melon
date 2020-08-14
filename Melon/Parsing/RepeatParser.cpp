#include "RepeatParser.h"

#include "StatementParser.h"
#include "ExpressionParser.h"

#include "Melon/Nodes/RepeatNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr RepeatParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Repeat) return nullptr;

	const UInt startIndex = info.index;
	const UInt repeatLine = info.Current().line;
	info.index++;

	Pointer<RepeatNode> repeatNode = new RepeatNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));

	if (NodePtr nodes = StatementParser::ParseMultiple(info)) {
		repeatNode->content = nodes;
	}

	if (info.Current().type == TokenType::Until) {
		info.index++;

		if (NodePtr cond = ExpressionParser::Parse(info)) {
			repeatNode->condition = cond;
			info.statementNumber++;
			return repeatNode;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("expression", "'until'", "repeat loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAtEnd("'until'", "'repeat'", repeatLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}

	info.index = startIndex;
	return nullptr;
}
