#include "LoopParser.h"

#include "ConditionParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "AssignmentParser.h"

#include "Melon/Nodes/ForConditionNode.h"
#include "Melon/Nodes/IntegerNode.h"

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

		ls.also = IsLoopAlso(type);
		ls.type = GetLoopType(type);

		if (!loop->segments.IsEmpty()) {
			if (IsLoopStart(type)) {
				return Parser::UnexpectedToken(info);
			}

			if (loop->segments.Size() > 1) {
				if (!loop->segments.Last().also && ls.also) {
					return Parser::UnexpectedToken(info);
				}

				if (loop->segments.Last().also == ls.also && loop->segments.Last().type == LoopNode::LoopType::None) {
					return Parser::UnexpectedToken(info);
				}
			}
		}

		info.index++;

		if (ls.type == LoopNode::LoopType::If) {
			ParseIf(ls, value, info);
		}
		else if (ls.type == LoopNode::LoopType::While) {
			ParseWhile(ls, value, info);
		}
		else if (ls.type == LoopNode::LoopType::For) {
			ParseFor(ls, value, info);
		}
		else {
			ParseNone(ls, value, info);
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

void LoopParser::ParseIf(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scopes = info.scopes.AddNext("if");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

	if (NodePtr cond = ConditionParser::Parse(info)) {
		info.statementNumber++;

		if (info.Current().type != TokenType::Then)
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'then'", "condition", "if segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

		info.index++;

		ls.condition = cond;
		info.scopeCount++;
		ls.statements = StatementParser::ParseMultiple(info);
		info.scopeCount--;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'" + value + "'", "if segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}
}

void LoopParser::ParseWhile(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scopes = info.scopes.AddNext("while");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

	if (NodePtr cond = ConditionParser::Parse(info)) {
		info.statementNumber++;

		if (info.Current().type != TokenType::Do)
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'do'", "condition", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

		info.index++;

		ls.condition = cond;
		info.loops++;
		info.scopeCount++;
		ls.statements = StatementParser::ParseMultiple(info);
		info.loops--;
		info.scopeCount--;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "'" + value + "'", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}
}

void LoopParser::ParseFor(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scopes = info.scopes.AddNext("for");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

	if (NodePtr init = AssignmentParser::Parse(info, true)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Comma) {
			info.index++;

			if (NodePtr cond = ConditionParser::Parse(info)) {
				Pointer<ForConditionNode> fcn = new ForConditionNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				fcn->loopInit = init;
				fcn->loopCondition = cond;

				if (info.Current().type == TokenType::Comma) {
					info.index++;

					if (NodePtr step = ExpressionParser::Parse(info)) {
						fcn->loopStep = step;
					}
					else if (NodePtr step = AssignmentParser::Parse(info, true)) {
						fcn->loopStep = step;
					}
					else {
						ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("expression or assignment", "','", "for segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
					}
				}
				else {
					Pointer<IntegerNode> num = new IntegerNode(FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
					num->number = 1;
					fcn->loopStep = num;
				}

				if (info.Current().type != TokenType::Do)
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'do'", "condition", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

				info.index++;

				ls.condition = fcn;
				info.loops++;
				info.scopeCount++;
				ls.statements = StatementParser::ParseMultiple(info);
				info.loops--;
				info.scopeCount--;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("condition", "','", "for segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("','", "'" + info.Current(-1).value + "'", "while segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("assignment", "'" + value + "'", "for segment"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}
}

void LoopParser::ParseNone(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scopes = info.scopes.AddNext(ls.also ? "also" : "else");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));

	info.scopeCount++;
	ls.statements = StatementParser::ParseMultiple(info);
	info.scopeCount--;
}

bool LoopParser::IsLoop(const TokenType t) {
	return IsLoopStart(t) || IsLoopMiddle(t);
}

bool LoopParser::IsLoopStart(const TokenType t) {
	return 
		t == TokenType::If ||
		t == TokenType::While ||
		t == TokenType::For;
}

bool LoopParser::IsLoopMiddle(const TokenType t) {
	return 
		t == TokenType::AlsoIf ||
		t == TokenType::ElseIf ||
		t == TokenType::AlsoWhile ||
		t == TokenType::ElseWhile ||
		t == TokenType::AlsoFor ||
		t == TokenType::ElseFor ||
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
	else if (t == TokenType::For || t == TokenType::AlsoFor || t == TokenType::ElseFor) {
		return LoopNode::LoopType::For;
	}

	return LoopNode::LoopType::None;
}

bool LoopParser::IsLoopAlso(const TokenType t) {
	return 
		t == TokenType::AlsoIf ||
		t == TokenType::AlsoWhile ||
		t == TokenType::AlsoFor ||
		t == TokenType::Also;
}