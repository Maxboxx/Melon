#include "TemplateParser.h"

#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<List<NameList>> TemplateParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Less) return nullptr;

	const UInt startIndex = info.index;
	info.index++;

	List<NameList> types;

	if (Optional<NameList> type = TypeParser::Parse(info)) {
		types.Add(*type);

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (Optional<NameList> type = TypeParser::Parse(info)) {
				types.Add(*type);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "type", LogMessage::Quote(","), "template argument list"), info.GetFileInfoPrev());
			}
		}
	}

	if (info.Current().type == TokenType::BShiftRight) {
		info.tokens[info.index].type  = TokenType::Greater;
		info.tokens[info.index].value = Name::Greater.name;
		info.tokens.Insert(info.index, info.tokens[info.index]);
	}

	if (info.Current().type != TokenType::Greater) {
		info.index = startIndex;
		return nullptr;
	}

	info.index++;
	return types;
}

Optional<List<NameList>> TemplateParser::ParseDefine(ParsingInfo& info) {
	if (info.Current().type != TokenType::Less) return nullptr;

	const UInt startIndex = info.index;
	info.index++;

	List<NameList> types;

	if (info.Current().type == TokenType::Name) {
		types.Add(NameList().Add(Name()).Add(Name(info.Current().value)));
		info.index++;

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (info.Current().type == TokenType::Name) {
				types.Add(NameList().Add(Name("")).Add(Name(info.Current().value)));
				info.index++;
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "type", LogMessage::Quote(","), "template argument list"), info.GetFileInfoPrev());
			}
		}
	}

	if (info.Current().type != TokenType::Greater) {
		info.index = startIndex;
		return nullptr;
	}

	if (types.IsEmpty()) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "type", LogMessage::Quote("<"), "template argument list"), info.GetFileInfoPrev());
		info.index = startIndex;
		return nullptr;
	}

	info.index++;
	return types;
}
