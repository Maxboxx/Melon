#include "MemberVariableParser.h"

#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

Optional<MemberVariableParser::Variables> MemberVariableParser::Parse(ParsingInfo& info) {
	Variables variables;

	const UInt startIndex = info.index;

	if (Optional<NameList> type = TypeParser::Parse(info)) {
		variables.types.Add(*type);

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (Optional<NameList> type = TypeParser::Parse(info)) {
				variables.types.Add(*type);
			}
			else {
				info.index = startIndex;
				return nullptr;
			}
		}
	}
	else {
		return nullptr;
	}

	if (info.Current().type != TokenType::Colon) {
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	if (info.Current().type != TokenType::Name) {
		info.index = startIndex;
		return nullptr;
	}

	variables.names.Add(Name(info.Current().value));
	info.index++;

	while (info.Current().type == TokenType::Comma) {
		info.index++;

		if (info.Current().type == TokenType::Name) {
			variables.names.Add(Name(info.Current().value));
			info.index++;
		}
		else {
			info.index = startIndex;
			return nullptr;
		}
	}

	if (variables.names.Count() > 1 && variables.types.Count() == 1) {
		while (variables.types.Count() < variables.names.Count()) {
			variables.types.Add(variables.types[0]);
		}
	}

	if (variables.names.Count() != variables.types.Count()) {
		info.index = startIndex;
		return nullptr;
	}

	return variables;
}
