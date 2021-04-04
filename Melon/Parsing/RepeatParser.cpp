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

	Pointer<RepeatNode> repeatNode = new RepeatNode(info.scope, info.GetFileInfo(info.Current(-1).line));

	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(repeatLine));

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
			info.scope = info.scope->Parent<ScopeSymbol>();
			return repeatNode;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", "'until'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.close_at", "'until'", "repeat", repeatLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	info.scope = info.scope->Parent<ScopeSymbol>();
	info.index = startIndex;
	return nullptr;
}
