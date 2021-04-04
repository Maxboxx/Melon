#include "DoParser.h"

#include "StatementParser.h"

#include "Melon/Nodes/DoNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr DoParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Do) {
		UInt line = info.Current().line;
		info.index++;

		info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(line));

		Pointer<DoNode> node = new DoNode(info.scope, info.GetFileInfo(line));

		info.scopeCount++;

		if (info.Current().type == TokenType::Arrow) {
			info.index++;

			if (NodePtr statement = StatementParser::Parse(info)) {
				node->nodes = statement;
				info.scopeCount--;
				info.scope = info.scope->Parent<ScopeSymbol>();
				info.statementNumber++;
				return node;
			}
			else {
				info.scopeCount--;
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				return nullptr;
			}
		}

		node->nodes = StatementParser::ParseMultiple(info);
		info.scopeCount--;

		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "do", line), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}

		info.index++;

		info.scope = info.scope->Parent<ScopeSymbol>();
		info.statementNumber++;
		return node;
	}

	return nullptr;
}