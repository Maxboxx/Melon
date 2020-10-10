#include "TemplateParser.h"

#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<List<ScopeList>> TemplateParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Less) return nullptr;

	const UInt startIndex = info.index;
	info.index++;

	List<ScopeList> types;

	if (Optional<ScopeList> type = TypeParser::Parse(info)) {
		types.Add((ScopeList)type);

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (Optional<ScopeList> type = TypeParser::Parse(info)) {
				types.Add((ScopeList)type);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("type", "','", "template list"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
	}

	if (info.Current().type == TokenType::BShiftRight) {
		info.tokens[info.index].type  = TokenType::Greater;
		info.tokens[info.index].value = Scope::Greater.name;
		info.tokens.Insert(info.index, info.tokens[info.index]);
	}

	if (info.Current().type != TokenType::Greater) {
		//ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'>'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		info.index = startIndex;
		return nullptr;
	}

	info.index++;
	return types;
}

Optional<List<Scope>> TemplateParser::ParseDefine(ParsingInfo& info) {
	if (info.Current().type != TokenType::Less) return nullptr;

	const UInt startIndex = info.index;
	info.index++;

	List<Scope> types;

	if (info.Current().type == TokenType::Name) {
		types.Add(Scope(info.Current().value));
		info.index++;

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (info.Current().type == TokenType::Name) {
				types.Add(Scope(info.Current().value));
				info.index++;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("type", "','", "template list"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
	}

	if (info.Current().type != TokenType::Greater) {
		//ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'>'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		info.index = startIndex;
		return nullptr;
	}

	if (types.IsEmpty()) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("type", "'<'", "template list"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		info.index = startIndex;
		return nullptr;
	}

	info.index++;
	return types;
}