#include "GuardParser.h"

#include "ConditionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/GuardNode.h"

#include "Melon/Symbols/Symbols.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NodePtr GuardParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (info.Current().type != TokenType::Guard) {
		return nullptr;
	}

	const UInt guardLine = info.Current().line;

	info.index++;

	if (NodePtr node = ConditionParser::Parse(info)) {
		info.scopes = info.scopes.AddNext("guard");
		Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, guardLine, info.statementNumber));

		Pointer<GuardNode> gn = new GuardNode(info.scopes, FileInfo(info.filename, guardLine, info.statementNumber));
		gn->cond = node;

		if (info.Current().type == TokenType::Else) {
			info.index++;
			info.scopeCount++;

			if (NodePtr statements = StatementParser::ParseMultiple(info)) {
				gn->else_ = statements;
			}

			info.scopeCount--;

			if (info.Current().type != TokenType::End) {
				ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("guard statement", guardLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				info.index = startIndex;
				return nullptr;
			}

			info.index++;
		}
		else if (info.Current().type == TokenType::Arrow) {
			info.index++;
			info.scopeCount++;

			if (NodePtr statement = StatementParser::Parse(info)) {
				gn->else_ = statement;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("statement", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			info.scopeCount--;
		}

		info.scopes = info.scopes.Pop();
		info.statementNumber++;
		return gn;
	}

	info.index = startIndex;
	return nullptr;
}