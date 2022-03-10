#include "LoopParser.h"

#include "ConditionParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "AssignmentParser.h"
#include "ScopeParser.h"

#include "Melon/Nodes/Integer.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<LoopStatement> LoopParser::Parse(ParsingInfo& info) {
	if (!IsLoopStart(info.Current().type)) return nullptr;

	Ptr<LoopStatement> loop = new LoopStatement(info.scope, info.GetFileInfo());

	const UInt startLine = info.Current().line;
	const String start = info.Current().value;
	bool single = false;

	while (IsValidSegmentType(info.Current().type, loop)) {
		LoopStatement::LoopSegment ls;
		TokenType type = info.Current().type;
		String value = info.Current().value;

		ls.also = IsLoopAlso(type);
		ls.type = GetLoopType(type);

		info.index++;

		single = ParseSegment(ls, value, info);

		loop->segments.Add(ls);

		info.scope = info.scope->Parent<ScopeSymbol>();
	}

	if (!single) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", LogMessage::Quote(start), startLine), info.GetFileInfoPrev());
		}

		info.index++;
	}

	info.statementNumber++;
	return loop;
}

bool LoopParser::ParseSegment(LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	switch (ls.type) {
		case LoopStatement::LoopType::If:    return ParseIf(ls, value, info);
		case LoopStatement::LoopType::While: return ParseWhile(ls, value, info);
		case LoopStatement::LoopType::For:   return ParseFor(ls, value, info);

		default: return ParseNone(ls, value, info);
	}
}

bool LoopParser::ParseIf(LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (Ptr<Condition> cond = ConditionParser::Parse(info)) {
		info.statementNumber++;
		ls.condition = cond;

		const bool single = info.Current().type == TokenType::Arrow;

		info.scopeCount++;
		ls.statements = ScopeParser::ParseNoEnd(info, TokenType::Then, ScopeParser::Info("then", "if condition"), true);
		info.scopeCount--;

		return single;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote(value)), info.GetFileInfoPrev());
	}

	return false;
}

bool LoopParser::ParseWhile(LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (Ptr<Condition> cond = ConditionParser::Parse(info)) {
		info.statementNumber++;
		ls.condition = cond;
		
		const bool single = info.Current().type == TokenType::Arrow;

		info.scopeCount++;
		ls.statements = ScopeParser::ParseNoEnd(info, TokenType::Do, ScopeParser::Info("do", "while condition"), true);
		info.scopeCount--;

		return single;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote(value)), info.GetFileInfoPrev());
	}

	return false;
}

bool LoopParser::ParseFor(LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (Ptr<Assignment> init = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Comma) {
			info.index++;

			Tuple<Optional<Name>, Ptr<Condition>> cond = ParseForCondition(info);

			if (cond.value2) {
				ls.init = init;
				ls.conditionOperator = cond.value1;
				ls.condition = cond.value2;

				if (info.Current().type == TokenType::Comma) {
					info.index++;

					Tuple<Optional<Name>, Ptr<Node>> step = ParseForStep(info);
					ls.stepOperator = step.value1;
					ls.step = step.value2;
				}
				else {
					Ptr<Integer> num = new Integer(info.GetFileInfoPrev());
					num->number = 1;
					ls.step = num;
					ls.stepOperator = Name::Add;
				}

				const bool single = info.Current().type == TokenType::Arrow;

				info.scopeCount++;
				ls.statements = ScopeParser::ParseNoEnd(info, TokenType::Do, ScopeParser::Info("do", "for condition"), true);
				info.scopeCount--;

				return single;
			}
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", LogMessage::Quote(","), LogMessage::Quote(info.Prev().value), "for loop"), info.GetFileInfoPrev());
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "assignment", LogMessage::Quote(value), "for loop"), info.GetFileInfoPrev());
	}

	return false;
}

Tuple<Optional<Name>, Ptr<Condition>> LoopParser::ParseForCondition(ParsingInfo& info) {
	Optional<Name> op;
	Ptr<Condition> node = nullptr;

	if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
		op = Name(info.Current().value);
		info.index++;

		node = Condition::FromExpression(ExpressionParser::Parse(info));

		if (!node) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Prev().value), "for loop"), info.GetFileInfoPrev());
		}
	}
	else if (Ptr<Condition> cond = ConditionParser::Parse(info)) {
		node = cond;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "condition", LogMessage::Quote(","), "for loop"), info.GetFileInfoPrev());
	}

	return Tuple<>::Create(op, node);
}

Tuple<Optional<Name>, Ptr<Node>> LoopParser::ParseForStep(ParsingInfo& info) {
	Optional<Name> op;
	Ptr<Node> node = nullptr;

	if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
		op = Name(info.Current().value);
		info.index++;

		if (Ptr<Expression> step = ExpressionParser::Parse(info)) {
			node = step;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Prev().value), "for loop"), info.GetFileInfoPrev());
		}
	}
	else if (Ptr<Assignment> step = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
		node = step;
	}
	else if (Ptr<Expression> step = ExpressionParser::Parse(info)) {
		node = step;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression or assignment", LogMessage::Quote(","), "for loop"), info.GetFileInfoPrev());
	}

	return Tuple<>::Create(op, node);
}

bool LoopParser::ParseNone(LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	const bool single = info.Current().type == TokenType::Arrow;

	info.scopeCount++;
	ls.statements = ScopeParser::ParseNoEnd(info, TokenType::None, ScopeParser::Info(), true);
	info.scopeCount--;

	return single;
}

bool LoopParser::IsValidSegmentType(const TokenType t, Weak<Nodes::LoopStatement> loop) {
	if (!IsLoop(t)) return false;

	if (IsLoopStart(t)) {
		return loop->segments.IsEmpty();
	}
	else if (loop->segments.IsEmpty()) {
		return false;
	}
	else if (IsLoopAlso(t)) {
		return loop->segments.Last().type != LoopStatement::LoopType::None && (loop->segments.Size() == 1 || loop->segments.Last().also);
	}
	else {
		return loop->segments.Last().also || loop->segments.Last().type != LoopStatement::LoopType::None;
	}
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

LoopStatement::LoopType LoopParser::GetLoopType(const TokenType t) {
	if (t == TokenType::If || t == TokenType::AlsoIf || t == TokenType::ElseIf) {
		return LoopStatement::LoopType::If;
	}
	else if (t == TokenType::While || t == TokenType::AlsoWhile || t == TokenType::ElseWhile) {
		return LoopStatement::LoopType::While;
	}
	else if (t == TokenType::For || t == TokenType::AlsoFor || t == TokenType::ElseFor) {
		return LoopStatement::LoopType::For;
	}

	return LoopStatement::LoopType::None;
}

bool LoopParser::IsLoopAlso(const TokenType t) {
	return 
		t == TokenType::AlsoIf ||
		t == TokenType::AlsoWhile ||
		t == TokenType::AlsoFor ||
		t == TokenType::Also;
}
