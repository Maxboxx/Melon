#include "LoopParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr LoopParser::Parse(ParsingInfo& info) {
	if (!IsLoopStart(info.Current().type)) return nullptr;

	Pointer<LoopNode> loop = new LoopNode(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

	const UInt startLine = info.Current().line;
	const String start = info.Current().value;

	while (IsLoop(info.Current().type)) {
		LoopNode::LoopSegment ls;
		TokenType type = info.Current().type;
		String value = info.Current().value;

		if (!loop->segments.IsEmpty() && IsLoopStart(type)) {
			return Parser::UnexpectedToken(info);
		}

		info.index++;

		ls.also = IsLoopAlso(type);
		ls.type = GetLoopType(type);

		if (ls.type == LoopNode::LoopType::If) {
			info.scopes = info.scopes.AddNext("if");
			Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

			if (NodePtr cond = ExpressionParser::Parse(info)) {
				info.statementNumber++;

				if (info.Current().type != TokenType::Then)
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'then'", "condition", "if segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

				info.index++;

				ls.condition = cond;
				ls.statements = StatementParser::ParseMultiple(info);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'" + value + "'", "if segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
		else if (ls.type == LoopNode::LoopType::While) {
			info.scopes = info.scopes.AddNext("while");
			Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

			if (NodePtr cond = ExpressionParser::Parse(info)) {
				info.statementNumber++;

				if (info.Current().type != TokenType::Do)
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'do'", "condition", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

				info.index++;

				ls.condition = cond;
				info.loops++;
				ls.statements = StatementParser::ParseMultiple(info);
				info.loops--;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'" + value + "'", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
		else {
			info.scopes = info.scopes.AddNext(ls.also ? "also" : "else");
			Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

			ls.statements = StatementParser::ParseMultiple(info);
		}

		loop->segments.Add(ls);

		info.scopes = info.scopes.Pop();
	}

	if (info.Current().type != TokenType::End)
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected(start, startLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

	info.index++;
	info.statementNumber++;
	return loop;
}

bool LoopParser::IsLoop(const TokenType t) {
	return IsLoopStart(t) || IsLoopMiddle(t);
}

bool LoopParser::IsLoopStart(const TokenType t) {
	return 
		t == TokenType::If ||
		t == TokenType::While;
}

bool LoopParser::IsLoopMiddle(const TokenType t) {
	return 
		t == TokenType::AlsoIf ||
		t == TokenType::ElseIf ||
		t == TokenType::AlsoWhile ||
		t == TokenType::ElseWhile ||
		t == TokenType::Also ||
		t == TokenType::Else;
}

LoopNode::LoopType LoopParser::GetLoopType(const TokenType t) {
	if (t == TokenType::If || t == TokenType::AlsoIf || t == TokenType::ElseIf) {
		return LoopNode::LoopType::If;
	}
	else if (t == TokenType::While || t == TokenType::AlsoWhile || t == TokenType::ElseWhile) {
		return LoopNode::LoopType::While;
	}

	return LoopNode::LoopType::None;
}

bool LoopParser::IsLoopAlso(const TokenType t) {
	return 
		t == TokenType::AlsoIf ||
		t == TokenType::AlsoWhile ||
		t == TokenType::Also;
}