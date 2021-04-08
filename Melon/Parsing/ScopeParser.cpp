#include "ScopeParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/ConvertNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const String& block, const UInt blockLine) {
	if (NodePtr node = ParseBlock(info, scopeStart, block, blockLine)) {
		return node;
	}
	else if (NodePtr node = ParseSingle(info)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseBlock(ParsingInfo& info, const TokenType scopeStart, const String& block, const UInt blockLine) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
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

NodePtr ScopeParser::ParseSingle(ParsingInfo& info) {
	if (info.Current().type != TokenType::Arrow) return nullptr;

	info.index++;

	if (NodePtr node = StatementParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}

NodePtr ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const String& block, const UInt blockLine) {
	if (NodePtr node = ParseExpressionBlock(info, scopeStart, block, blockLine)) {
		return node;
	}
	else if (NodePtr node = ParseExpressionSingle(info)) {
		return node;
	}

	return nullptr;
}

NodePtr ScopeParser::ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const String& block, const UInt blockLine) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		return nullptr;
	}

	NodePtr node = ExpressionParser::Parse(info);

	if (node == nullptr) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		return nullptr;
	}

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", block, blockLine), info.GetFileInfoPrev());
		return nullptr;
	}

	info.index++;
	return node;
}

NodePtr ScopeParser::ParseExpressionSingle(ParsingInfo& info) {
	if (info.Current().type != TokenType::Arrow) return nullptr;

	info.index++;

	if (NodePtr node = ExpressionParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}
