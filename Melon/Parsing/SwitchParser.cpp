#include "SwitchParser.h"

#include "ExpressionParser.h"
#include "ScopeParser.h"

#include "Melon/Nodes/SwitchNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr SwitchParser::ParseStatement(ParsingInfo& info) {
	if (info.Current().type != TokenType::Switch) return nullptr;

	const UInt startIndex = info.index;
	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
	}

	Pointer<SwitchStatement> switchNode = new SwitchStatement(info.scope, info.GetFileInfo(switchLine));
	switchNode->expr = false;
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

NodePtr SwitchParser::ParseExpression(ParsingInfo& info, const bool returnOnError) {
	if (info.Current().type != TokenType::Switch) return nullptr;

	const UInt startIndex = info.index;
	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);
	bool error = false;

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
		error = true;
	}

	Pointer<SwitchStatement> switchNode = new SwitchStatement(info.scope, info.GetFileInfo(switchLine));
	switchNode->expr = true;
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

List<NodePtr> SwitchParser::ParseCaseExpressions(ParsingInfo& info, bool& error) {
	List<NodePtr> expressions;

	while (NodePtr node = ExpressionParser::Parse(info)) {
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

void SwitchParser::ParseStatementCases(ParsingInfo& info, Pointer<SwitchStatement>& switchNode) {
	while (ParseStatementCase(info, switchNode) || ParseStatementDefault(info, switchNode));
}

bool SwitchParser::ParseStatementCase(ParsingInfo& info, Pointer<SwitchStatement>& switchNode) {
	if (info.Current().type != TokenType::Case) return false;
	
	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	bool error;
	List<NodePtr> cases = ParseCaseExpressions(info, error);

	info.scopeCount++;

	NodePtr node = ScopeParser::Parse(info, TokenType::Then, ScopeParser::Info("then", "switch case", "switch case", line), true);

	switchNode->nodes.Add(node);
	switchNode->cases.Add(cases);

	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool SwitchParser::ParseStatementDefault(ParsingInfo& info, Pointer<SwitchStatement>& switchNode) {
	if (info.Current().type != TokenType::Default) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
	}

	info.index++;
	info.scopeCount++;

	NodePtr node = ScopeParser::Parse(info, TokenType::None, ScopeParser::Info("default case", line), true);

	switchNode->def = node;
	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

void SwitchParser::ParseExpressionCases(ParsingInfo& info, Pointer<SwitchStatement>& switchNode, bool& error) {
	while (ParseExpressionCase(info, switchNode, error) || ParseExpressionDefault(info, switchNode, error));
}

bool SwitchParser::ParseExpressionCase(ParsingInfo& info, Pointer<SwitchStatement>& switchNode, bool& error) {
	if (info.Current().type != TokenType::Case) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	List<NodePtr> cases = ParseCaseExpressions(info, error);

	info.scopeCount++;

	NodePtr node = ScopeParser::ParseExpression(info, TokenType::Then, ScopeParser::Info("then", "switch case", "switch case", line), true);

	if (!node) error = true;

	switchNode->nodes.Add(node);
	switchNode->cases.Add(cases);

	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool SwitchParser::ParseExpressionDefault(ParsingInfo& info, Pointer<SwitchStatement>& switchNode, bool& error) {
	if (info.Current().type != TokenType::Default) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
		error = true;
	}

	info.index++;
	info.scopeCount++;

	NodePtr node = ScopeParser::ParseExpression(info, TokenType::None, ScopeParser::Info("default case", line), true);

	if (!node) error = true;

	switchNode->def = node;
	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}
