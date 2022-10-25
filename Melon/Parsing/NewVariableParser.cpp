#include "NewVariableParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"

#include "Melon/Nodes/DiscardExpression.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<NewVariableNode> NewVariableParser::Parse(ParsingInfo& info, const bool single) {
	static Regex upper = Regex("^%u");
	static Regex underscore = Regex("%a_+%a");

	const UInt startIndex = info.index;

	Ptr<NewVariableNode> node = new NewVariableNode(info.scope, info.GetFileInfo());

	while (const Optional<NameList> type = ParseType(info)) {
		node->types.Add(*type);

		if (single && node->types.Count() == 1) break;

		if (info.Current().type == TokenType::Comma) {
			info.index++;
		}
		else {
			break;
		}
	}

	if (node->types.IsEmpty()) return nullptr;

	if (info.Current().type == TokenType::Colon) {
		while (true) {
			info.index++;
			node->attributes.Add(VariableAttributeParser::Parse(info));

			if (info.Current().type != TokenType::Name && info.Current().type != TokenType::Discard) {
				if (node->attributes.Last() != VariableAttributes::None) {
					ErrorLog::Error(LogMessage("error.syntax.expected.name.var", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				}

				break;
			}

			node->names.Add(Name(info.Current().value));

			if (info.Current().type == TokenType::Name) {
				if (upper.Match(info.Current().value)) {
					ErrorLog::Info(LogMessage("info.name.lower", "variable", info.Current().value), info.GetFileInfo());
				}

				if (underscore.Match(info.Current().value)) {
					ErrorLog::Info(LogMessage("info.name.under", "variable", info.Current().value), info.GetFileInfo());
				}
			}

			if (info.Next().type != TokenType::Comma) break;

			if (node->types.Count() > 1 && node->names.Count() >= node->types.Count()) {
				Parser::UnexpectedToken(info);
				return nullptr;
			}
		}

		if (node->names.Count() < node->types.Count()) {
			ErrorLog::Error(LogMessage("error.syntax.assign.var.few"), info.GetFileInfoPrev());
		}

		return node;
	}

	info.index = startIndex;
	return nullptr;
}

Optional<NameList> NewVariableParser::ParseType(ParsingInfo& info) {
	if (info.Current().type == TokenType::Discard) {
		info.index++;
		return NameList::Discard;
	}

	return TypeParser::Parse(info);
}