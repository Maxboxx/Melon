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

	if (info.Current().type != TokenType::If) return nullptr;

	const UInt ifLine = info.Current().line;
	info.index++;

	NodePtr cond = ConditionParser::Parse(info);

	bool error = false;

	if (!cond) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", "'if'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		error = true;
	}

	bool single = info.Current().type == TokenType::Arrow;

	if (!single && info.Current().type != TokenType::Then) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "'then'", "condition", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		error = true;
	}

	info.index++;

	if (NodePtr expr = ExpressionParser::Parse(info)) {
		Pointer<IfExprNode> ifexpr = new IfExprNode(info.scope, info.GetFileInfo(ifLine));
		ifexpr->nodes.Add(expr);
		ifexpr->conditions.Add(cond);

		if (!single) while (info.Current().type == TokenType::ElseIf) {
			info.index++;

			NodePtr cond = ConditionParser::Parse(info);

			if (!cond) {
				ErrorLog::Error(LogMessage("error.sytax.expected.after", "condition", "'elseif'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				error = true;
			}

			if (info.Current().type != TokenType::Then) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "'then'", "condition", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				error = true;
			}

			info.index++;

			if (NodePtr expr = ExpressionParser::Parse(info)) {
				ifexpr->conditions.Add(cond);
				ifexpr->nodes.Add(expr);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", "'then'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				error = true;
			}
		}

		if (info.Current().type == TokenType::Else) {
			info.index++;

			if (NodePtr expr = ExpressionParser::Parse(info)) {
				if (single || info.Current().type == TokenType::End) {
					if (!single) info.index++;
					ifexpr->nodes.Add(expr);

					if (error && returnOnError) return nullptr;
					return ifexpr;
				}

				ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "if expression", ifLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				error = true;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", "'else'", "if expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			error = true;
		}
	}

	ErrorLog::Error(LogMessage("error.syntax.if.required.else"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	error = true;
	info.index = startIndex;
	return nullptr;
}