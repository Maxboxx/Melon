#include "ScopeParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/ConvertNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	if (NodePtr node = ParseSingle(info)) {
		return node;
	}
	else if (NodePtr node = ParseBlock(info, scopeStart, expected, block, blockLine, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseBlock(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(expected), blockLine), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	NodePtr statements = StatementParser::ParseMultiple(info);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", block, blockLine), info.GetFileInfoPrev());
		return nullptr;
	}

	info.index++;
	return statements;
}

NodePtr ScopeParser::ParseSingle(ParsingInfo& info, const bool required) {
	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), info.Prev().value), info.GetFileInfoPrev());
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

NodePtr ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	if (NodePtr node = ParseExpressionSingle(info)) {
		return node;
	}
	else if (NodePtr node = ParseExpressionBlock(info, scopeStart, expected, block, blockLine, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	const UInt startIndex = info.index;

	NodePtr node = ParseExpressionBlockNoEnd(info, scopeStart, expected, block, blockLine, required);

	if (node == nullptr) {
		info.index = startIndex;
		return nullptr;
	}

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", block, blockLine), info.GetFileInfoPrev());
		return nullptr;
	}

	info.index++;
	return node;
}

NodePtr ScopeParser::ParseExpressionSingle(ParsingInfo& info, const bool required) {
	const UInt startIndex = info.index;

	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), info.Prev().value), info.GetFileInfoPrev());
		}
		
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	if (NodePtr node = ExpressionParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	info.index = startIndex;
	return nullptr;
}

NodePtr ScopeParser::ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	if (NodePtr node = ParseExpressionSingle(info)) {
		return node;
	}
	else if (NodePtr node = ParseExpressionBlockNoEnd(info, scopeStart, expected, block, blockLine, required)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseExpressionBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const String& expected, const String& block, const UInt blockLine, const bool required) {
	const UInt startIndex = info.index;
	
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(expected), block), info.GetFileInfoPrev());
		}

		info.index = startIndex;
		return nullptr;
	}

	NodePtr node = ExpressionParser::Parse(info);

	if (node == nullptr) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		info.index = startIndex;
		return nullptr;
	}

	return node;
}
