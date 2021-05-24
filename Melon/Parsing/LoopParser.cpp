#include "LoopParser.h"

#include "ConditionParser.h"
#include "ExpressionParser.h"
#include "StatementParser.h"
#include "AssignmentParser.h"
#include "ScopeParser.h"

#include "Melon/Nodes/ForConditionNode.h"
#include "Melon/Nodes/IntegerNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr LoopParser::Parse(ParsingInfo& info) {
	if (!IsLoopStart(info.Current().type)) return nullptr;

	Pointer<LoopNode> loop = new LoopNode(info.scope, info.GetFileInfo());

	const UInt startLine = info.Current().line;
	const String start = info.Current().value;
	bool single = false;

	while (IsValidSegmentType(info.Current().type, loop)) {
		LoopNode::LoopSegment ls;
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

bool LoopParser::ParseSegment(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	switch (ls.type) {
		case LoopNode::LoopType::If:    return ParseIf(ls, value, info);
		case LoopNode::LoopType::While: return ParseWhile(ls, value, info);
		case LoopNode::LoopType::For:   return ParseFor(ls, value, info);

		default: return ParseNone(ls, value, info);
	}
}

bool LoopParser::ParseIf(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (NodePtr cond = ConditionParser::Parse(info)) {
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

bool LoopParser::ParseWhile(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (NodePtr cond = ConditionParser::Parse(info)) {
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

bool LoopParser::ParseFor(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	if (NodePtr init = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Comma) {
			info.index++;

			Tuple<Optional<Name>, NodePtr> cond = ParseForCondition(info);

			if (cond.value2) {
				Pointer<ForConditionNode> fcn = new ForConditionNode(info.scope, info.GetFileInfoPrev());
				fcn->loopInit = init;
				fcn->conditionOperator = cond.value1;
				fcn->loopCondition = cond.value2;

				if (info.Current().type == TokenType::Comma) {
					info.index++;

					Tuple<Optional<Name>, NodePtr> step = ParseForStep(info);
					fcn->stepOperator = step.value1;
					fcn->loopStep = step.value2;
				}
				else {
					Pointer<IntegerNode> num = new IntegerNode(info.GetFileInfoPrev());
					num->number = 1;
					fcn->loopStep = num;
					fcn->stepOperator = Name::Add;
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

Tuple<Optional<Name>, NodePtr> LoopParser::ParseForCondition(ParsingInfo& info) {
	Optional<Name> op;
	NodePtr node = nullptr;

	if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
		op = Name(info.Current().value);
		info.index++;

		node = ExpressionParser::Parse(info);

		if (!node) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Prev().value), "for loop"), info.GetFileInfoPrev());
		}
	}
	else if (NodePtr cond = ConditionParser::Parse(info)) {
		node = cond;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "condition", LogMessage::Quote(","), "for loop"), info.GetFileInfoPrev());
	}

	return Tuple<>::Create(op, node);
}

Tuple<Optional<Name>, NodePtr> LoopParser::ParseForStep(ParsingInfo& info) {
	Optional<Name> op;
	NodePtr node = nullptr;

	if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
		op = Name(info.Current().value);
		info.index++;

		if (NodePtr step = ExpressionParser::Parse(info)) {
			node = step;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Prev().value), "for loop"), info.GetFileInfoPrev());
		}
	}
	else if (NodePtr step = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
		node = step;
	}
	else if (NodePtr step = ExpressionParser::Parse(info)) {
		node = step;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression or assignment", LogMessage::Quote(","), "for loop"), info.GetFileInfoPrev());
	}

	return Tuple<>::Create(op, node);
}

bool LoopParser::ParseNone(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo());

	const bool single = info.Current().type == TokenType::Arrow;

	info.scopeCount++;
	ls.statements = ScopeParser::ParseNoEnd(info, TokenType::None, ScopeParser::Info(), true);
	info.scopeCount--;

	return single;
}

bool LoopParser::IsValidSegmentType(const TokenType t, const Boxx::Pointer<Nodes::LoopNode>& loop) {
	if (!IsLoop(t)) return false;

	if (IsLoopStart(t)) {
		return loop->segments.IsEmpty();
	}
	else if (loop->segments.IsEmpty()) {
		return false;
	}
	else if (IsLoopAlso(t)) {
		return loop->segments.Last().type != LoopNode::LoopType::None && (loop->segments.Size() == 1 || loop->segments.Last().also);
	}
	else {
		return loop->segments.Last().also || loop->segments.Last().type != LoopNode::LoopType::None;
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
