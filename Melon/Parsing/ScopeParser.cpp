#include "ScopeParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/TypeConversion.h"
#include "Melon/Nodes/ReturnStatement.h"

#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<Statements> ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	bool needsEnd;
	return Parse(info, scopeStart, scopeInfo, required, true, needsEnd);
}

Ptr<Statements> ScopeParser::ParseNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool& needsEnd, const bool required) {
	return Parse(info, scopeStart, scopeInfo, required, false, needsEnd);
}

Ptr<Statements> ScopeParser::Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required, bool parseEnd, bool& needsEnd) {
	switch (info.Current().type) {
		case TokenType::Colon: {
			info.index++;

			if (Ptr<Statement> statement = StatementParser::Parse(info)) {
				Ptr<Statements> statements = new Statements(statement->scope, statement->File());
				statements->statements.Add(statement);
				needsEnd = false;
				return statements;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(":")), info.GetFileInfoPrev());
			break;
		}

		case TokenType::Arrow: {
			info.index++;

			if (Ptr<Expression> expr = ExpressionParser::Parse(info)) {
				Ptr<Statements> statements = new Statements(info.scope, expr->File());
				Ptr<ReturnStatement> ret = new ReturnStatement(info.scope, expr->File());
				ret->values.Add(expr);
				ret->func = info.scope->CurrentFunction()->AbsoluteName();
				statements->statements.Add(ret);
				needsEnd = false;
				return statements;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.after", "expression", LogMessage::Quote("->")), info.GetFileInfoPrev());
			break;
		}

		default: {
			const UInt index = info.index;

			if (scopeStart != TokenType::None) {
				if (info.Current().type != scopeStart) {
					if (required) {
						ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(scopeInfo.expected), scopeInfo.after), info.GetFileInfoPrev());
					}

					break;
				}

				info.index++;
			}

			Ptr<Statements> statements = StatementParser::ParseMultiple(info);

			if (!parseEnd) return statements;

			if (info.Current().type == TokenType::End) {
				info.index++;
				needsEnd = false;
				return statements;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			info.index = index;

			break;
		}
	}

	needsEnd = false;
	return nullptr;
}

Ptr<Expression> ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required) {
	bool needsEnd;
	return ParseExpression(info, scopeStart, scopeInfo, required, true, needsEnd);
}

Ptr<Expression> ScopeParser::ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool& needsEnd, const bool required) {
	return ParseExpression(info, scopeStart, scopeInfo, required, false, needsEnd);
}

Ptr<Expression> ScopeParser::ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required, bool parseEnd, bool& needsEnd) {
	switch (info.Current().type) {
		case TokenType::Colon: {
			info.index++;

			if (Ptr<Expression> expr = ExpressionParser::Parse(info)) {
				needsEnd = false;
				return expr;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(":")), info.GetFileInfoPrev());
			break;
		}

		default: {
			const UInt index = info.index;

			if (scopeStart != TokenType::None) {
				if (info.Current().type != scopeStart) {
					if (required) {
						ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(scopeInfo.expected), scopeInfo.after), info.GetFileInfoPrev());
					}

					break;
				}

				info.index++;
			}

			Ptr<Expression> expr = ExpressionParser::Parse(info);

			if (!parseEnd) return expr;

			if (info.Current().type == TokenType::End) {
				info.index++;
				needsEnd = false;
				return expr;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", scopeInfo.block, scopeInfo.blockLine), info.GetFileInfoPrev());
			info.index = index;

			break;
		}
	}

	needsEnd = false;
	return nullptr;
}
