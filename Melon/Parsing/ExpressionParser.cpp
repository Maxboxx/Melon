#include "ExpressionParser.h"

#include "IntegerParser.h"
#include "BooleanParser.h"
#include "IfExpressionParser.h"
#include "MatchParser.h"
#include "NameParser.h"
#include "CallParser.h"
#include "DotParser.h"
#include "ObjectInitParser.h"
#include "TypeParser.h"
#include "AsParser.h"

#include "Melon/Nodes/BinaryOperatorExpression.h"
#include "Melon/Nodes/UnaryOperatorExpression.h"
#include "Melon/Nodes/LogicExpression.h"
#include "Melon/Nodes/CallExpression.h"
#include "Melon/Nodes/DotExpression.h"
#include "Melon/Nodes/ObjectInitExpression.h"
#include "Melon/Nodes/NameExpression.h"
#include "Melon/Nodes/NilValue.h"
#include "Melon/Nodes/DefaultExpression.h"
#include "Melon/Nodes/SafeUnwrapExpression.h"
#include "Melon/Nodes/SafeUnwrapChain.h"
#include "Melon/Nodes/TypeConversion.h"
#include "Melon/Nodes/IfExpression.h"
#include "Melon/Nodes/MatchExpression.h"
#include "Melon/Nodes/Integer.h"
#include "Melon/Nodes/Boolean.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<Expression> ExpressionParser::Parse(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;

	if (Ptr<Expression> valueNode = ParseBinaryOperand(info, statement)) {
		List<Pair<TokenType, Ptr<BinaryOperatorExpression>>> operators;
		List<Ptr<Expression>> nodes;
		nodes.Add(valueNode);
		UByte highestPrecedence = 0;

		while (!info.EndOfFile()) {
			Token token = info.Current();

			if (IsBinaryOperator(token.type)) {
				if (token.type == TokenType::As) {
					if (Ptr<TypeConversion> node = AsParser::Parse(info)) {
						node->expression = nodes.Last();
						nodes[nodes.Count() - 1] = node;
					}
				}
				else {
					info.index++;

					if (Ptr<Expression> valueNode2 = ParseBinaryOperand(info)) {
						Ptr<BinaryOperatorExpression> node;

						if (IsLogic(token.type)) {
							node = new LogicExpression(info.scope, token.type, info.GetFileInfo(token.line));
						}
						else if (token.type == TokenType::DoubleQuestion) {
							node = new DefaultExpression(info.scope, info.GetFileInfo(token.line));
						}
						else {
							node = new BinaryOperatorExpression(info.scope, Name(token.value), info.GetFileInfo(token.line));
						}

						operators.Add(Pair<TokenType, Ptr<BinaryOperatorExpression>>(token.type, node));
						nodes.Add(valueNode2);

						if (highestPrecedence < Precedence(token.type)) highestPrecedence = Precedence(token.type);
					}
					else {
						info.index--;
						break;
					}
				}
			}
			else {
				break;
			}
		}

		if (!operators.IsEmpty()) {
			while (!operators.IsEmpty()) {
				for (UInt i = 0; i < operators.Count();) {
					if (Precedence(operators[i].key) == highestPrecedence) {
						Ptr<BinaryOperatorExpression> op = operators[i].value;
						op->operand1 = nodes[i];
						op->operand2 = nodes[i + 1];
						nodes[i] = op;

						operators.RemoveAt(i);
						nodes.RemoveAt(i + 1);
					}
					else {
						i++;
					}
				}

				highestPrecedence--;
			}
		}

		return nodes[0];
	}

	info.index = startIndex;
	return nullptr;
}

UByte ExpressionParser::Precedence(const TokenType op) {
	switch (op) {
		case TokenType::As: return 8;

		case TokenType::DoubleQuestion: return 7;

		case TokenType::Mul:  return 6;
		case TokenType::Div:  return 6;
		case TokenType::IDiv: return 6;
		case TokenType::Mod:  return 6;

		case TokenType::Plus:  return 5;
		case TokenType::Minus: return 5;

		case TokenType::BShiftLeft:  return 4;
		case TokenType::BShiftRight: return 4;

		case TokenType::BOr:   return 3;
		case TokenType::BAnd:  return 3;
		case TokenType::BXor:  return 3;
		case TokenType::BNor:  return 3;
		case TokenType::BNand: return 3;
		case TokenType::BXnor: return 3;

		case TokenType::Equal:     return 2;
		case TokenType::NotEqual:  return 2;
		case TokenType::LessEq:    return 2;
		case TokenType::GreaterEq: return 2;
		case TokenType::Less:      return 2;
		case TokenType::Greater:   return 2;

		case TokenType::Or:   return 1;
		case TokenType::And:  return 1;
		case TokenType::Xor:  return 1;
		case TokenType::Nor:  return 1;
		case TokenType::Nand: return 1;
		case TokenType::Xnor: return 1;
		default: return 0;
	}
}

bool ExpressionParser::IsBinaryOperator(const TokenType op) {
	switch (op) {
		case TokenType::As: return true;

		case TokenType::DoubleQuestion: return true;

		case TokenType::Mul:  return true;
		case TokenType::Div:  return true;
		case TokenType::IDiv: return true;
		case TokenType::Mod:  return true;

		case TokenType::Plus:  return true;
		case TokenType::Minus: return true;

		case TokenType::BShiftLeft:  return true;
		case TokenType::BShiftRight: return true;

		case TokenType::BOr:   return true;
		case TokenType::BAnd:  return true;
		case TokenType::BXor:  return true;
		case TokenType::BNor:  return true;
		case TokenType::BNand: return true;
		case TokenType::BXnor: return true;

		case TokenType::Equal:     return true;
		case TokenType::NotEqual:  return true;
		case TokenType::LessEq:    return true;
		case TokenType::GreaterEq: return true;
		case TokenType::Less:      return true;
		case TokenType::Greater:   return true;

		case TokenType::Or:   return true;
		case TokenType::And:  return true;
		case TokenType::Xor:  return true;
		case TokenType::Nor:  return true;
		case TokenType::Nand: return true;
		case TokenType::Xnor: return true;
		default: return false;
	}
}

bool ExpressionParser::IsLogic(const TokenType type) {
	return 
		type == TokenType::Or   ||
		type == TokenType::And  ||
		type == TokenType::Xor  ||
		type == TokenType::Nor  ||
		type == TokenType::Nand ||
		type == TokenType::Xnor;
}	

Ptr<Expression> ExpressionParser::ParseBinaryOperand(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;
	const Token token = info.Current();

	if (Ptr<Expression> node = ParseUnaryOperand(info, statement)) {
		return node;
	}
	else if (token.type == TokenType::Minus || token.type == TokenType::BNot || token.type == TokenType::Not) {
		info.index++;

		if (Ptr<Expression> node = ParseBinaryOperand(info)) {
			Ptr<UnaryOperatorExpression> opNode = new UnaryOperatorExpression(info.scope, Name(token.value), info.GetFileInfo(token.line));

			opNode->operand = node;
			return opNode;
		}
	}

	info.index = startIndex;
	return nullptr;
}

Ptr<Expression> ExpressionParser::ParseUnaryOperand(ParsingInfo& info, const bool statement) {
	if (Ptr<Expression> node = ParseChainOperand(info, statement)) {
		bool hasSafeUnwrap = false;

		while (!info.EndOfFile()) {
			if (Ptr<CallExpression> call = CallParser::Parse(info)) {
				call->expression = node;
				node = call;

				continue;
			}

			if (Ptr<DotExpression> dot = DotParser::Parse(info)) {
				dot->expression = node;
				node = dot;
				continue;
			}

			if (Ptr<ObjectInitExpression> dot = ObjectInitParser::Parse(info)) {
				dot->expression = node;
				node = dot;
				continue;
			}

			if (info.Current().type == TokenType::Exclamation) {
				Ptr<UnaryOperatorExpression> unwrap = new UnaryOperatorExpression(info.scope, Name::Unwrap, info.GetFileInfo());
				unwrap->operand = node;
				info.index++;
				node = unwrap;
				continue;
			}

			if (info.Current().type == TokenType::Question) {
				hasSafeUnwrap = true;

				Ptr<SafeUnwrapExpression> unwrap = new SafeUnwrapExpression(info.scope, info.GetFileInfo());
				unwrap->expression = node;
				info.index++;
				node = unwrap;
				continue;
			}

			break;
		}

		if (hasSafeUnwrap) {
			Ptr<SafeUnwrapChain> unwrap = new SafeUnwrapChain(info.scope, info.GetFileInfoPrev());
			unwrap->expression = node;
			node = unwrap;
		}

		return node;
	}

	return nullptr;
}

Ptr<Expression> ExpressionParser::ParseChainOperand(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;
	const UInt startLine  = info.Current().line;

	if (statement) {
		ErrorLog::AddMarker();

		try {
			if (Ptr<IfExpression> node = IfExpressionParser::Parse(info, true)) {
				ErrorLog::RemoveMarker();
				return node;
			}
		}
		catch (CompileError& e) {
			info.index = startIndex;
		}

		try {
			if (Ptr<MatchExpression> node = MatchParser::ParseExpression(info, true)) {
				ErrorLog::RemoveMarker();
				return node;
			}
		}
		catch (CompileError& e) {
			info.index = startIndex;
		}

		ErrorLog::Revert();
	}
	else {
		if (Ptr<IfExpression> node = IfExpressionParser::Parse(info)) {
			return node;
		}
		else if (Ptr<MatchExpression> node = MatchParser::ParseExpression(info)) {
			return node;
		}
	}

	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;
		Ptr<Expression> node = Parse(info);

		if (info.Current().type == TokenType::ParenClose) {
			info.index++;
			return node;
		}
	}
	else if (Ptr<Integer> node = IntegerParser::Parse(info)) {
		return node;
	}
	else if (Ptr<Boolean> node = BooleanParser::Parse(info)) {
		return node;
	}
	else if (info.Current().type == TokenType::Nil) {
		Ptr<NilValue> node = new NilValue(info.GetFileInfo());
		info.index++;
		return node;
	}

	if (info.Current().type == TokenType::Global) {
		const UInt line = info.Current().line;
		info.index++;

		if (Ptr<DotExpression> dn = DotParser::Parse(info)) {
			Ptr<NameExpression> nn = new NameExpression(nullptr, info.GetFileInfo(line));
			nn->name = Name::Global;
			dn->expression = nn;
			return dn;
		}

		if (info.Current().type == TokenType::Dot) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("."), LogMessage::Quote("global")), info.GetFileInfoPrev());
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", LogMessage::Quote(".")), info.GetFileInfoPrev());
		}

		info.index = startIndex;
		return nullptr;
	}
	else if (Optional<Name> node = TypeParser::ParseName(info)) {
		Ptr<NameExpression> nn = new NameExpression(info.scope, info.GetFileInfo(startLine));
		nn->name = *node;
		return nn;
	}
	else if (Ptr<NameExpression> node = NameParser::Parse(info)) {
		return node;
	}

	info.index = startIndex;
	return nullptr;
}
