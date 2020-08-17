#include "DoParser.h"

#include "StatementParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr DoParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Do) {
		UInt line = info.Current().line;
		info.index++;

		info.scopes = info.scopes.AddNext("do");
		Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));
		
		info.scopeCount++;
		NodePtr statements = StatementParser::ParseMultiple(info);
		info.scopeCount--;

		if (info.Current().type != TokenType::End)
			ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("do", line), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

		info.index++;

		info.scopes = info.scopes.Pop();
		info.statementNumber++;
		return statements;
	}

	return nullptr;
}