#include "IfExpressionParser.h"

#include "ConditionParser.h"
#include "ExpressionParser.h"
#include "ScopeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<IfExpression> IfExpressionParser::Parse(ParsingInfo& info, const bool returnOnError) {
	if (info.Current().type != TokenType::If) return nullptr;

	const UInt startIndex = info.index;
	const UInt ifLine = info.Current().line;
	info.index++;

	// Parse condition
	Ptr<Condition> cond = ConditionParser::Parse(info);

	bool error = false;

	if (!cond) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote("if")), info.GetFileInfoPrev());
		error = true;
		info.index = startIndex;
		return nullptr;
	}

	// Setup node
	Ptr<IfExpression> ifexpr = new IfExpression(info.scope, info.GetFileInfo(ifLine));
	ifexpr->conditions.Add(cond);

	// Parse simple if expression
	/*if (ParseSimple(info, (IfExprNode*)ifexpr, ifLine, error)) {
		return ifexpr;
	}*/

	// Parse full if expression
	if (ParseFull(info, ifexpr, ifLine, error)) {
		return ifexpr;
	}

	info.index = startIndex;
	return nullptr;
}

bool IfExpressionParser::ParseSimple(ParsingInfo& info, Weak<IfExpression> node, const UInt line, bool& error) {
	if (info.Current().type != TokenType::Arrow) return false;
	info.index++;
	
	// Parse first expression
	Ptr<Expression> node1 = ExpressionParser::Parse(info);

	if (!node1) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(":"), "if expression"), info.GetFileInfoPrev());
		error = true;
		return false;
	}

	// Parse else keyword
	if (info.Current().type != TokenType::Else) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", LogMessage::Quote("else"), "expression", "if expression"), info.GetFileInfoPrev());
		error = true;
		return false;
	}

	info.index++;

	// Parse second expression
	Ptr<Expression> node2 = ExpressionParser::Parse(info);

	if (!node2) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote("else"), "if expression"), info.GetFileInfoPrev());
		error = true;
		return false;
	}

	node->nodes.Add(node1);
	node->nodes.Add(node2);
	return true;
}

bool IfExpressionParser::ParseFull(ParsingInfo& info, Weak<IfExpression> node, const UInt line, bool& error) {
	// Parse if block
	if (Ptr<Expression> expr = ScopeParser::ParseExpressionNoEnd(info, TokenType::Then, ScopeParser::Info("then", "if condition"), true)) {
		node->nodes.Add(expr);
	}
	else {
		error = true;
		return false;
	}

	// Parse elseif blocks
	while (info.Current().type == TokenType::ElseIf) {
		info.index++;

		if (Ptr<Condition> cond = ConditionParser::Parse(info)) {
			node->conditions.Add(cond);
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote("elseif")), info.GetFileInfoPrev());
			error = true;
			return false;
		}

		if (Ptr<Expression> expr = ScopeParser::ParseExpressionNoEnd(info, TokenType::Then, ScopeParser::Info("then", "if condition"), true)) {
			node->nodes.Add(expr);
		}
		else {
			error = true;
			return false;
		}
	}

	// Parse else block
	if (info.Current().type != TokenType::Else) {
		ErrorLog::Error(LogMessage("error.syntax.if.required.else"), info.GetFileInfoPrev());
		error = true;
		return false;
	}

	info.index++;

	if (Ptr<Expression> expr = ScopeParser::ParseExpression(info, TokenType::None, ScopeParser::Info(LogMessage::Quote("else"), info.Prev().line), true)) {
		node->nodes.Add(expr);
	}
	else {
		error = true;
		return false;
	}

	return true;
}
