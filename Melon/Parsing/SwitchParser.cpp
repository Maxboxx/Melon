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

	Pointer<SwitchNode> switchNode = new SwitchNode(info.scope, info.GetFileInfo(switchLine));
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

NodePtr SwitchParser::ParseExpression(ParsingInfo& info) {
	if (info.Current().type != TokenType::Switch) return nullptr;

	const UInt startIndex = info.index;
	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
	}

	Pointer<SwitchNode> switchNode = new SwitchNode(info.scope, info.GetFileInfo(switchLine));
	switchNode->expr = true;
	switchNode->match = value;

	ParseExpressionCases(info, switchNode);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch expression", switchLine), info.GetFileInfoPrev());
	}
	else {
		info.index++;
	}

	info.statementNumber++;
	return switchNode;
}

List<NodePtr> SwitchParser::ParseCaseExpressions(ParsingInfo& info) {
	List<NodePtr> expressions;

	while (NodePtr node = ExpressionParser::Parse(info)) {
		expressions.Add(node);

		if (info.Current().type != TokenType::Comma) break;
		info.index++;
	}

	if (expressions.IsEmpty()) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "case expression", LogMessage::Quote("case")), info.GetFileInfoPrev());
	}

	return expressions;
}

void SwitchParser::ParseStatementCases(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	while (ParseStatementCase(info, switchNode) || ParseStatementDefault(info, switchNode));
}

bool SwitchParser::ParseStatementCase(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	if (info.Current().type != TokenType::Case) return false;
	
	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	List<NodePtr> cases = ParseCaseExpressions(info);

	info.scopeCount++;

	NodePtr node = ScopeParser::Parse(info, TokenType::Then, "switch case", line);

	if (!node) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("then"), "case expression"), info.GetFileInfoPrev());
	}

	switchNode->nodes.Add(node);
	switchNode->cases.Add(cases);

	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool SwitchParser::ParseStatementDefault(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	if (info.Current().type != TokenType::Default) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
	}

	info.index++;
	info.scopeCount++;

	NodePtr node = ScopeParser::Parse(info, TokenType::None, "default case", line);

	switchNode->def = node;
	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

void SwitchParser::ParseExpressionCases(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	while (ParseExpressionCase(info, switchNode) || ParseExpressionDefault(info, switchNode));
}

bool SwitchParser::ParseExpressionCase(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	if (info.Current().type != TokenType::Case) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
	info.index++;

	List<NodePtr> cases = ParseCaseExpressions(info);

	info.scopeCount++;

	NodePtr node = ScopeParser::ParseExpression(info, TokenType::Then, "switch case", line);

	switchNode->nodes.Add(node);
	switchNode->cases.Add(cases);

	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

bool SwitchParser::ParseExpressionDefault(ParsingInfo& info, Pointer<SwitchNode>& switchNode) {
	if (info.Current().type != TokenType::Default) return false;

	const UInt line = info.Current().line;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (switchNode->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), info.GetFileInfo());
	}

	info.index++;
	info.scopeCount++;

	NodePtr node = ScopeParser::ParseExpression(info, TokenType::None, "default case", line);

	switchNode->def = node;
	info.scopeCount--;
	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}
