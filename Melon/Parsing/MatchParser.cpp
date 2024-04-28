#include "MatchParser.h"

#include "ExpressionParser.h"
#include "ScopeParser.h"
#include "TypeParser.h"
#include "NameParser.h"

#include "Melon/Nodes/TypeConversion.h"
#include "Melon/Nodes/NameExpression.h"

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
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("match")), info.GetFileInfoPrev());
	}

	if (info.Current().type != TokenType::Do) {
		ErrorLog::Error(LogMessage("Missing do on match"), info.GetFileInfoPrev());
	}
	else {
		info.index++;
	}

	Ptr<MatchStatement> switchNode = new MatchStatement(info.scope, info.GetFileInfo(switchLine));
	switchNode->match = value;

	ParseStatementCases(info, switchNode);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "match statement", switchLine), info.GetFileInfoPrev());
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
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("match")), info.GetFileInfoPrev());
		error = true;
	}

	if (info.Current().type != TokenType::Do) {
		ErrorLog::Error(LogMessage("Missing do on match"), info.GetFileInfoPrev());
		error = true;
	}
	else {
		info.index++;
	}

	Ptr<MatchExpression> switchNode = new MatchExpression(info.scope, info.GetFileInfo(switchLine));
	switchNode->match = value;

	ParseExpressionCases(info, switchNode, error);

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "match expression", switchLine), info.GetFileInfoPrev());
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
	const UInt line = info.Current().line;
	const UInt startIndex = info.index;
	
	if (info.Current().type == TokenType::Is) {
		bool error;

		info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());
		info.index++;

		switchNode->cases.Add(ParseCaseExpressions(info, error));

		info.scopeCount++;
		switchNode->nodes.Add(ScopeParser::Parse(info, TokenType::Do, ScopeParser::Info("do", "case", "case", line), true));
		info.scopeCount--;

		info.scope = info.scope->Parent<ScopeSymbol>();
		return true;
	}
	else if (Optional<NameList> type = TypeParser::Parse(info)) {
		if (info.Current().type != TokenType::Colon) {
			info.index = startIndex;
			return false;
		}
	
		info.index++;

		if (info.Current().type != TokenType::Name) {
			info.index = startIndex;
			return false;
		}

		Name varName = Name(info.Current().value);

		VariableSymbol* varSym = new VariableSymbol(info.GetFileInfo());
		varSym->type = *type;

		info.index++;

		if (info.Current().type != TokenType::As) {
			info.index = startIndex;
			return false;
		}
		
		info.index++;

		if (Optional<NameList> valType = TypeParser::Parse(info)) {
			info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

			info.statementNumber++;

			List<Ptr<Expression>> caseList;

			Ptr<TypeConversion> conv = new TypeConversion(info.scope, info.GetFileInfo());

			Ptr<NameExpression> name = new NameExpression(info.scope, info.GetFileInfo());
			name->name = varName;

			conv->expression = name;
			conv->isExplicit = true;
			conv->type = *valType;

			caseList.Add(conv);

			switchNode->cases.Add(caseList);

			info.scopeCount++;
			switchNode->nodes.Add(ScopeParser::Parse(info, TokenType::Do, ScopeParser::Info("do", "case", "case", line), true));
			info.scopeCount--;

			info.scope->AddSymbol(varName, varSym);

			info.scope = info.scope->Parent<ScopeSymbol>();
			return true;
		}
	}

	info.index = startIndex;
	return false;
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
	switchNode->def = ScopeParser::Parse(info, TokenType::None, ScopeParser::Info("else", line), true);
	info.scopeCount--;

	info.scope = info.scope->Parent<ScopeSymbol>();
	return true;
}

void MatchParser::ParseExpressionCases(ParsingInfo& info, Ptr<MatchExpression>& switchNode, bool& error) {
	while (ParseExpressionCase(info, switchNode, error) || ParseExpressionDefault(info, switchNode, error));
}

bool MatchParser::ParseExpressionCase(ParsingInfo& info, Ptr<MatchExpression>& switchNode, bool& error) {
	const UInt line = info.Current().line;
	const UInt startIndex = info.index;

	if (info.Current().type == TokenType::Is) {
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
	else if (Optional<NameList> type = TypeParser::Parse(info)) {
		if (info.Current().type != TokenType::Colon) {
			info.index = startIndex;
			return false;
		}

		info.index++;

		if (info.Current().type != TokenType::Name) {
			info.index = startIndex;
			return false;
		}

		Name varName = Name(info.Current().value);

		VariableSymbol* varSym = new VariableSymbol(info.GetFileInfo());
		varSym->type = *type;

		info.index++;

		if (info.Current().type != TokenType::As) {
			info.index = startIndex;
			return false;
		}

		info.index++;

		if (Optional<NameList> valType = TypeParser::Parse(info)) {
			info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

			info.statementNumber++;

			List<Ptr<Expression>> caseList;

			Ptr<TypeConversion> conv = new TypeConversion(info.scope, info.GetFileInfo());

			Ptr<NameExpression> name = new NameExpression(info.scope, info.GetFileInfo());
			name->name = varName;

			conv->expression = name;
			conv->isExplicit = true;
			conv->type = *valType;

			caseList.Add(conv);

			switchNode->cases.Add(caseList);

			info.scopeCount++;

			Ptr<Expression> node = ScopeParser::ParseExpression(info, TokenType::Do, ScopeParser::Info("do", "switch case", "switch case", line));

			if (!node) error = true;

			switchNode->nodes.Add(node);

			info.scope->AddSymbol(varName, varSym);

			info.scopeCount--;
			info.scope = info.scope->Parent<ScopeSymbol>();
			return true;
		}
	}

	info.index = startIndex;
	return false;
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
