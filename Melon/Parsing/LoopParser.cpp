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

	Pointer<LoopNode> loop = new LoopNode(info.scope, info.GetFileInfo(info.Current().line));

	const UInt startLine = info.Current().line;
	const String start = info.Current().value;
	bool single = false;

	while (!single && IsLoop(info.Current().type)) {
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
			single = ParseIf(ls, value, info, loop->segments.Size());
		}
		else if (ls.type == LoopNode::LoopType::While) {
			single = ParseWhile(ls, value, info, loop->segments.Size());
		}
		else if (ls.type == LoopNode::LoopType::For) {
			single = ParseFor(ls, value, info, loop->segments.Size());
		}
		else {
			ParseNone(ls, value, info, loop->segments.Size());
		}

		loop->segments.Add(ls);

		info.scope = info.scope->Parent<ScopeSymbol>();
	}

	if (!single) {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", LogMessage::Quote(start), startLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}

		info.index++;
	}

	info.statementNumber++;
	return loop;
}

bool LoopParser::ParseIf(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const UInt index) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

	if (NodePtr cond = ConditionParser::Parse(info)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Then) {
			info.index++;

			ls.condition = cond;
			info.scopeCount++;
			ls.statements = StatementParser::ParseMultiple(info);
			info.scopeCount--;
			return false;
		}
		else if (index == 0 && info.Current().type == TokenType::Arrow) {
			info.index++;

			ls.condition = cond;
			info.scopeCount++;
			ls.statements = StatementParser::Parse(info);

			if (!ls.statements) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}

			info.scopeCount--;
			return true;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "'then'", "if condition"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote(value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	return false;
}

bool LoopParser::ParseWhile(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const UInt index) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

	if (NodePtr cond = ConditionParser::Parse(info)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Do) {
			info.index++;

			ls.condition = cond;
			info.loops++;
			info.scopeCount++;
			ls.statements = StatementParser::ParseMultiple(info);
			info.loops--;
			info.scopeCount--;
			return false;
		}
		else if (index == 0 && info.Current().type == TokenType::Arrow) {
			info.index++;

			ls.condition = cond;
			info.loops++;
			info.scopeCount++;
			ls.statements = StatementParser::Parse(info);

			if (!ls.statements) {
				ErrorLog::Error(LogMessage("error.sytax.expected.after", "statement", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}

			info.loops--;
			info.scopeCount--;
			return true;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "'do'", "while condition"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "condition", LogMessage::Quote(value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	return true;
}

bool LoopParser::ParseFor(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const UInt index) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

	if (NodePtr init = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
		info.statementNumber++;

		if (info.Current().type == TokenType::Comma) {
			info.index++;

			Optional<Name> op;
			NodePtr node = nullptr;
			
			if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
				op = Name(info.Current().value);
				info.index++;

				node = ExpressionParser::Parse(info);

				if (!node) {
					ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Current(-1).value), "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				}
			}
			else if (NodePtr cond = ConditionParser::Parse(info)) {
				node = cond;
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "condition", "','", "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}

			if (node) {
				Pointer<ForConditionNode> fcn = new ForConditionNode(info.scope, info.GetFileInfo(info.Current(-1).line));
				fcn->loopInit = init;
				fcn->conditionOperator = op;
				fcn->loopCondition = node;

				if (info.Current().type == TokenType::Comma) {
					info.index++;

					if (ExpressionParser::IsBinaryOperator(info.Current().type)) {
						fcn->stepOperator = Name(info.Current().value);
						info.index++;

						if (NodePtr step = ExpressionParser::Parse(info)) {
							fcn->loopStep = step;
						}
						else {
							ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression", LogMessage::Quote(info.Current(-1).value), "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
						}
					}
					else if (NodePtr step = AssignmentParser::Parse(info, AssignmentParser::Flags::Single)) {
						fcn->loopStep = step;
					}
					else if (NodePtr step = ExpressionParser::Parse(info)) {
						fcn->loopStep = step;
					}
					else {
						ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "expression or assignment", "','", "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
					}
				}
				else {
					Pointer<IntegerNode> num = new IntegerNode(FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
					num->number = 1;
					fcn->loopStep = num;
					fcn->stepOperator = Name::Add;
				}

				if (info.Current().type == TokenType::Do) {
					info.index++;

					ls.condition = fcn;
					info.loops++;
					info.scopeCount++;
					ls.statements = StatementParser::ParseMultiple(info);
					info.loops--;
					info.scopeCount--;
					return false;
				}
				else if (index == 0 && info.Current().type == TokenType::Arrow) {
					info.index++;

					ls.condition = fcn;
					info.loops++;
					info.scopeCount++;
					ls.statements = StatementParser::Parse(info);

					if (!ls.statements) {
						ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
					}

					info.loops--;
					info.scopeCount--;
					return true;
				}
				else {
					ErrorLog::Error(LogMessage("error.syntax.expected.after", "'do'", "for condition"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
				}
			}
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "','", LogMessage::Quote(info.Current(-1).value), "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "assignment", LogMessage::Quote(value), "for loop"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	return false;
}

void LoopParser::ParseNone(LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const UInt index) {
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

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