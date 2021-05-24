#include "ScopeParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/ConvertNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (NodePtr node = ParseSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (NodePtr node = ParseBlock(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (NodePtr node = ParseSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (NodePtr node = ParseBlockNoEnd(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	NodePtr statements = ParseBlockNoEnd(info, scopeStart, scopeInfo, required);

	if (statements) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			return nullptr;
		}

		info.index++;
	}

	return statements;
}

NodePtr ScopeParser::ParseBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(scopeInfo.expected), scopeInfo.after), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	return StatementParser::ParseMultiple(info);
}

NodePtr ScopeParser::ParseSingle(ParsingInfo& info, const Info& scopeInfo, const bool required) {
	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), scopeInfo.after), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	info.index++;

	if (NodePtr node = StatementParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}

NodePtr ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (NodePtr node = ParseExpressionSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (NodePtr node = ParseExpressionBlock(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (NodePtr node = ParseExpressionSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (NodePtr node = ParseExpressionBlockNoEnd(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	NodePtr node = ParseExpressionBlockNoEnd(info, scopeStart, scopeInfo, required);

	if (node) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			return nullptr;
		}

		info.index++;
	}

	return node;
}

NodePtr ScopeParser::ParseExpressionBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(scopeInfo.expected), scopeInfo.after), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	NodePtr node = ExpressionParser::Parse(info);

	if (node == nullptr) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		return nullptr;
	}

	return node;
}

NodePtr ScopeParser::ParseExpressionSingle(ParsingInfo& info, const Info& scopeInfo, const bool required) {
	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), info.Prev().value), info.GetFileInfoPrev());
		}
		
		return nullptr;
	}

	info.index++;

	if (NodePtr node = ExpressionParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}
