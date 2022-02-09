#include "RepeatParser.h"

#include "StatementParser.h"
#include "ConditionParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

Ptr<RepeatStatement> RepeatParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Repeat) return nullptr;

	const UInt startIndex = info.index;
	const UInt repeatLine = info.Current().line;
	info.index++;

	Ptr<RepeatStatement> repeatNode = new RepeatStatement(info.scope, info.GetFileInfoPrev());

	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(repeatLine));

	info.scopeCount++;
	info.loops++;
	repeatNode->statements = StatementParser::ParseMultiple(info);
	info.scopeCount--;
	info.loops--;

	if (info.Current().type == TokenType::Until) {
		info.index++;

		if (Ptr<Condition> cond = ConditionParser::Parse(info)) {
			repeatNode->condition = cond;
			info.statementNumber++;
			info.scope = info.scope->Parent<ScopeSymbol>();
			return repeatNode;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote("until")), info.GetFileInfoPrev());
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.close_at", LogMessage::Quote("until"), "repeat", repeatLine), info.GetFileInfoPrev());
	}

	info.scope = info.scope->Parent<ScopeSymbol>();
	info.index = startIndex;
	return nullptr;
}
