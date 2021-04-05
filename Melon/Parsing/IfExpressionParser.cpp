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
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote("if")), info.GetFileInfoPrev());
		error = true;
	}

	bool single = info.Current().type == TokenType::Arrow;

	if (!single && info.Current().type != TokenType::Then) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", LogMessage::Quote("then"), "condition", "if expression"), info.GetFileInfoPrev());
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
				ErrorLog::Error(LogMessage("error.sytax.expected.after", "condition", LogMessage::Quote("elseif")), info.GetFileInfoPrev());
				error = true;
			}

			if (info.Current().type != TokenType::Then) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", LogMessage::Quote("then"), "condition", "if expression"), info.GetFileInfoPrev());
				error = true;
			}

			info.index++;

			if (NodePtr expr = ExpressionParser::Parse(info)) {
				ifexpr->conditions.Add(cond);
				ifexpr->nodes.Add(expr);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote("then"), "if expression"), info.GetFileInfoPrev());
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

				ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "if expression", ifLine), info.GetFileInfoPrev());
				error = true;
			}

			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote("else"), "if expression"), info.GetFileInfoPrev());
			error = true;
		}
	}

	ErrorLog::Error(LogMessage("error.syntax.if.required.else"), info.GetFileInfoPrev());
	error = true;
	info.index = startIndex;
	return nullptr;
}
