#include "IfExpressionParser.h"

#include "ConditionParser.h"
#include "ExpressionParser.h"

#include "Melon/Nodes/IfExprNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr IfExpressionParser::Parse(ParsingInfo& info, const bool returnOnError) {
	const UInt startIndex = info.index;

	if (info.Current().type != TokenType::If) {
		return nullptr;
	}

	const UInt ifLine = info.Current().line;
	info.index++;

	NodePtr cond = ConditionParser::Parse(info);

	bool error = false;

	if (!cond) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'if'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		error = true;
	}

	if (info.Current().type != TokenType::Then) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'then'", "condition", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		error = true;
	}

	info.index++;

	if (NodePtr expr = ExpressionParser::Parse(info)) {
		Pointer<IfExprNode> ifexpr = new IfExprNode(info.scopes, FileInfo(info.filename, ifLine, info.statementNumber));
		ifexpr->nodes.Add(expr);
		ifexpr->conditions.Add(cond);

		while (info.Current().type == TokenType::ElseIf) {
			info.index++;

			NodePtr cond = ConditionParser::Parse(info);

			if (!cond) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'elseif'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}

			if (info.Current().type != TokenType::Then) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'then'", "condition", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}

			info.index++;

			if (NodePtr expr = ExpressionParser::Parse(info)) {
				ifexpr->conditions.Add(cond);
				ifexpr->nodes.Add(expr);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("expression", "'then'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}
		}

		if (info.Current().type == TokenType::Else) {
			info.index++;

			if (NodePtr expr = ExpressionParser::Parse(info)) {
				if (info.Current().type == TokenType::End) {
					info.index++;
					ifexpr->nodes.Add(expr);

					if (error && returnOnError) return nullptr;
					return ifexpr;
				}

				ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("if expression", ifLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}

			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("expression", "'else'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			error = true;
		}
	}

	ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAtEnd("'else'", "if expression", ifLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	error = true;
	info.index = startIndex;
	return nullptr;
}