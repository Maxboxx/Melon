#include "NewVariableParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"

#include "Melon/Nodes/NewVariableNode.h"
#include "Melon/Nodes/DiscardNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr NewVariableParser::Parse(ParsingInfo& info, const bool single) {
	static Regex upper = Regex("^%u");
	static Regex underscore = Regex("%a_+%a");

	const UInt startIndex = info.index;

	Pointer<NewVariableNode> node = new NewVariableNode(info.scope, info.GetFileInfo(info.Current().line));

	while (const Optional<ScopeList> type = ParseType(info)) {
		node->types.Add(*type);

		if (single && node->types.Size() == 1) break;

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
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("variable name", "attributes"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}

				break;
			}

			node->names.Add(Scope(info.Current().value));

			if (info.Current().type == TokenType::Name) {
				if (upper.Match(info.Current().value)) {
					ErrorLog::Info(InfoError(InfoError::LowerName("variable", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				if (underscore.Match(info.Current().value)) {
					ErrorLog::Info(InfoError(InfoError::LowerUnderscoreName("variable", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}
			}

			if (info.Next().type != TokenType::Comma) break;

			if (node->types.Size() > 1 && node->names.Size() >= node->types.Size()) {
				return Parser::UnexpectedToken(info);
			}
		}

		if (node->names.Size() < node->types.Size()) {
			ErrorLog::Error(SyntaxError(SyntaxError::FewVariables, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		return node;
	}

	info.index = startIndex;
	return nullptr;
}

Optional<ScopeList> NewVariableParser::ParseType(ParsingInfo& info) {
	if (info.Current().type == TokenType::Discard) {
		info.index++;
		return ScopeList::Discard;
	}

	return TypeParser::Parse(info);
}