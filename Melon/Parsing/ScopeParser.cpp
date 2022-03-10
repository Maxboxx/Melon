#include "ScopeParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/TypeConversion.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<Statements> ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (Ptr<Statements> node = ParseSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (Ptr<Statements> node = ParseBlock(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

Ptr<Statements> ScopeParser::ParseNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (Ptr<Statements> node = ParseSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (Ptr<Statements> node = ParseBlockNoEnd(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

Ptr<Statements> ScopeParser::ParseBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	Ptr<Statements> statements = ParseBlockNoEnd(info, scopeStart, scopeInfo, required);

	if (statements) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			return nullptr;
		}

		info.index++;
	}

	return statements;
}

Ptr<Statements> ScopeParser::ParseBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
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

Ptr<Statements> ScopeParser::ParseSingle(ParsingInfo& info, const Info& scopeInfo, const bool required) {
	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), scopeInfo.after), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	info.index++;

	if (Ptr<Statement> node = StatementParser::Parse(info)) {
		Ptr<Statements> statements = new Statements(node->scope, node->File());
		statements->statements.Add(node);
		return statements;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}

Ptr<Expression> ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (Ptr<Expression> node = ParseExpressionSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (Ptr<Expression> node = ParseExpressionBlock(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

Ptr<Expression> ScopeParser::ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (Ptr<Expression> node = ParseExpressionSingle(info, scopeInfo, false)) {
		return node;
	}
	else if (Ptr<Expression> node = ParseExpressionBlockNoEnd(info, scopeStart, scopeInfo, required)) {
		return node;
	}

	return nullptr;
}

Ptr<Expression> ScopeParser::ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	Ptr<Expression> node = ParseExpressionBlockNoEnd(info, scopeStart, scopeInfo, required);

	if (node) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			return nullptr;
		}

		info.index++;
	}

	return node;
}

Ptr<Expression> ScopeParser::ParseExpressionBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	if (info.Current().type == scopeStart) {
		info.index++;
	}
	else if (scopeStart != TokenType::None) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(scopeInfo.expected), scopeInfo.after), info.GetFileInfoPrev());
		}

		return nullptr;
	}

	Ptr<Expression> node = ExpressionParser::Parse(info);

	if (!node) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		return nullptr;
	}

	return node;
}

Ptr<Expression> ScopeParser::ParseExpressionSingle(ParsingInfo& info, const Info& scopeInfo, const bool required) {
	if (info.Current().type != TokenType::Arrow) {
		if (required) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("->"), info.Prev().value), info.GetFileInfoPrev());
		}
		
		return nullptr;
	}

	info.index++;

	if (Ptr<Expression> node = ExpressionParser::Parse(info)) {
		return node;
	}

	ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
	return nullptr;
}
