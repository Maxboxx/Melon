#include "RepeatParser.h"

#include "StatementParser.h"
#include "ConditionParser.h"

#include "Melon/Nodes/RepeatNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NodePtr RepeatParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Repeat) return nullptr;

	const UInt startIndex = info.index;
	const UInt repeatLine = info.Current().line;
	info.index++;

	Pointer<RepeatNode> repeatNode = new RepeatNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));

	info.scopes = info.scopes.AddNext("repeat");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, repeatLine, info.statementNumber));

	info.scopeCount++;
	info.loops++;
	repeatNode->content = StatementParser::ParseMultiple(info);
	info.scopeCount--;
	info.loops--;

	if (info.Current().type == TokenType::Until) {
		info.index++;

		if (NodePtr cond = ConditionParser::Parse(info)) {
			repeatNode->condition = cond;
			info.statementNumber++;
			info.scopes = info.scopes.Pop();
			return repeatNode;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'until'", "repeat loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAtEnd("'until'", "'repeat'", repeatLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}

	info.scopes = info.scopes.Pop();
	info.index = startIndex;
	return nullptr;
}
