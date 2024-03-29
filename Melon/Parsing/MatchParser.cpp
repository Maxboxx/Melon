#include "MatchParser.h"

#include "ExpressionParser.h"
#include "ScopeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<MatchStatement> MatchParser::ParseStatement(ParsingInfo& info) {
	if (info.Current().type != TokenType::Match) return nullptr;

	const UInt startIndex = info.index;
	const UInt switchLine = info.Current().line;
	info.index++;

	Ptr<Expression> value = ExpressionParser::Parse(info);

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
	}

	Ptr<MatchStatement> switchNode = new MatchStatement(info.scope, info.GetFileInfo(switchLine));
	switchNode->match = value;

	ParseStatementCases(info, switchNode);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch statement", switchLine), info.GetFileInfoPrev());
	}
	else {
		info.index++;
	}

	info.statementNumber++;
	return switchNode;
}

Ptr<MatchExpression> MatchParser::ParseExpression(ParsingInfo& info, const bool returnOnError) {
	if (info.Current().type != TokenType::Match) return nullptr;

	const UInt startIndex = info.index;
	const UInt switchLine = info.Current().line;
	info.index++;

	Ptr<Expression> value = ExpressionParser::Parse(info);
	bool error = false;

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
		error = true;
	}

	Ptr<MatchExpression> switchNode = new MatchExpression(info.scope, info.GetFileInfo(switchLine));
	switchNode->match = value;

	ParseExpressionCases(info, switchNode, error);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch expression", switchLine), info.GetFileInfoPrev());
		error = true;
	}
	else {
		info.index++;
	}

	if (returnOnError && error) {
		info.index = startIndex;
		return nullptr;
	}

	info.statementNumber++;
	return switchNode;
}

List<Ptr<Expression>> MatchParser::ParseCaseExpressions(ParsingInfo& info, bool& error) {
	List<Ptr<Expression>> expressions;

	while (Ptr<Expression> node = ExpressionParser::Parse(info)) {
		expressions.Add(node);

		if (info.Current().type != TokenType::Comma) break;
		info.index++;
	}

	if (expressions.IsEmpty()) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "case expression", LogMessage::Quote("case")), info.GetFileInfoPrev());
		error = true;
	}

	return expressions;
}

void MatchParser::ParseStatementCases(ParsingInfo& info, Ptr<MatchStatement>& switchNode) {
	while (ParseStatementCase(info, switchNode) || ParseStatementDefault(info, switchNode));
}

bool MatchParser::ParseStatementCase(ParsingInfo& info, Ptr<MatchStatement>& switchNode) {
	if (info.Current().type != TokenType::Is) return false;
	
	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	bool error;

	switchNode->cases.Add(ParseCaseExpressions(info, error));

	info.scopeCount++;
	switchNode->nodes.Add(ScopeParser::Parse(info, TokenType::Do, ScopeParser::Info("do", "switch case", "switch case", line), true));
	info.scopeCount--;

	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool MatchParser::ParseStatementDefault(ParsingInfo& info, Ptr<MatchStatement>& switchNode) {
	if (info.Current().type != TokenType::Else) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
	}

	info.index++;

	info.scopeCount++;
	switchNode->def = ScopeParser::Parse(info, TokenType::None, ScopeParser::Info("default case", line), true);
	info.scopeCount--;

	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

void MatchParser::ParseExpressionCases(ParsingInfo& info, Ptr<MatchExpression>& switchNode, bool& error) {
	while (ParseExpressionCase(info, switchNode, error) || ParseExpressionDefault(info, switchNode, error));
}

bool MatchParser::ParseExpressionCase(ParsingInfo& info, Ptr<MatchExpression>& switchNode, bool& error) {
	if (info.Current().type != TokenType::Is) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	switchNode->cases.Add(ParseCaseExpressions(info, error));

	info.scopeCount++;

	Ptr<Expression> node = ScopeParser::ParseExpression(info, TokenType::Do, ScopeParser::Info("do", "switch case", "switch case", line));

	if (!node) error = true;

	switchNode->nodes.Add(node);

	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool MatchParser::ParseExpressionDefault(ParsingInfo& info, Ptr<MatchExpression>& switchNode, bool& error) {
	if (info.Current().type != TokenType::Else) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
		error = true;
	}

	info.index++;
	info.scopeCount++;

	Ptr<Expression> node = ScopeParser::ParseExpression(info, TokenType::None, ScopeParser::Info("default case", line));

	if (!node) error = true;

	switchNode->def = node;
	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}
