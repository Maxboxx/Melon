#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<ScopeList> TypeParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Name || info.Current().type == TokenType::Global) {
		ScopeList type = ScopeList().Add(Scope(info.Current().value));
		info.index++;

		while (true) {
			if (info.Current().type != TokenType::Dot) break;
			if (info.Current(1).type != TokenType::Name) break;

			type = type.Add(Scope(info.Current(1).value));
			info.index += 2;
		}

		if (type.Size() == 1 && type[0] == Scope::Global) {
			if (info.Current().type == TokenType::Dot) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'.'", "'global'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("name", "'.'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			return nullptr;
		}

		return type;
	}

	return nullptr;
}