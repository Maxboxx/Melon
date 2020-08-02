#include "ExpressionParser.h"

#include "IntegerParser.h"
#include "BooleanParser.h"
#include "IfExpressionParser.h"
#include "SwitchExpressionParser.h"
#include "NameParser.h"
#include "CallParser.h"
#include "MethodCallParser.h"
#include "DotParser.h"
#include "CustomInitParser.h"

#include "Melon/Nodes/BinaryOperatorNode.h"
#include "Melon/Nodes/UnaryOperatorNode.h"
#include "Melon/Nodes/LogicNode.h"
#include "Melon/Nodes/CallNode.h"
#include "Melon/Nodes/DotNode.h"
#include "Melon/Nodes/CustomInitNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ExpressionParser::Parse(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;

	if (NodePtr valueNode = ParseValue(info, statement)) {
		List<Pair<TokenType, Pointer<BinaryOperatorNode>>> operators;
		List<NodePtr> nodes;
		nodes.Add(valueNode);
		UByte highestPrecedence = 0;

		while (!info.EndOfFile()) {
			Token token = info.Current();

			if (IsBinaryOperator(token.type)) {
				info.index++;

				if (NodePtr valueNode2 = ParseValue(info)) {
					Pointer<BinaryOperatorNode> node;

					if (IsLogic(token.type)) {
						node = new LogicNode(info.scopes, token.type, FileInfo(info.filename, token.line));
					}
					else {
						node = new BinaryOperatorNode(info.scopes, Scope(token.value), FileInfo(info.filename, token.line));
					}

					operators.Add(Pair<TokenType, Pointer<BinaryOperatorNode>>(token.type, node));
					nodes.Add(valueNode2);

					if (highestPrecedence < Precedence(token.type)) highestPrecedence = Precedence(token.type);
				}
				else {
					info.index--;
					break;
				}
			}
			else {
				break;
			}
		}

		if (!operators.IsEmpty()) {
			while (!operators.IsEmpty()) {
				for (UInt i = 0; i < operators.Size();) {
					if (Precedence(operators[i].key) == highestPrecedence) {
						Pointer<BinaryOperatorNode> op = operators[i].value;
						op->node1 = nodes[i];
						op->node2 = nodes[i + 1];
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

			return nodes[0];
		}

		return valueNode;
	}

	info.index = startIndex;
	return nullptr;
}

UByte ExpressionParser::Precedence(const TokenType op) {
	switch (op) {
		//case TokenType::As return 8;

		case TokenType::Pow: return 7;

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
		//case TokenType::As return true;

		case TokenType::Pow: return true;

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

NodePtr ExpressionParser::ParseValue(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;
	const Token token = info.Current();

	if (NodePtr node = ParseSingleValue(info, statement)) {
		return node;
	}
	else if (token.type == TokenType::Minus || token.type == TokenType::BXor) {
		info.index++;

		if (NodePtr node = ParseValue(info)) {
			Pointer<UnaryOperatorNode> opNode = new UnaryOperatorNode(info.scopes, Scope(token.value), FileInfo(info.filename, token.line));

			opNode->node = node;
			return opNode;
		}
	}

	info.index = startIndex;
	return nullptr;
}

NodePtr ExpressionParser::ParseRawValue(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;

	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;
		NodePtr node = Parse(info);

		if (info.Current().type == TokenType::ParenClose) {
			info.index++;
			return node;
		}
	}
	else if (NodePtr node = IntegerParser::Parse(info)) {
		return node;
	}
	else if (NodePtr node = BooleanParser::Parse(info)) {
		return node;
	}

	if (statement) {
		ErrorLog::AddMark();

		try {
			if (NodePtr node = IfExpressionParser::Parse(info, true)) {
				ErrorLog::RemoveMark();
				return node;
			}
		}
		catch (CompileError e) {
			info.index = startIndex;
		}

		try {
			if (NodePtr node = SwitchExpressionParser::Parse(info, true)) {
				ErrorLog::RemoveMark();
				return node;
			}
		}
		catch (CompileError e) {
			info.index = startIndex;
		}

		ErrorLog::RevertToMark();
		ErrorLog::RemoveMark();
	}
	else {
		if (NodePtr node = IfExpressionParser::Parse(info)) {
			return node;
		}
		else if (NodePtr node = SwitchExpressionParser::Parse(info)) {
			return node;
		}
	}

	if (NodePtr node = NameParser::Parse(info)) {
		return node;
	}

	info.index = startIndex;
	return nullptr;
}

NodePtr ExpressionParser::ParseSingleValue(ParsingInfo& info, const bool statement) {
	if (NodePtr node = ParseRawValue(info, statement)) {
		while (!info.EndOfFile()) {
			if (NodePtr call = CallParser::Parse(info)) {
				call.Cast<CallNode>()->node = node;
				node = call;

				continue;
			}

			if (NodePtr call = MethodCallParser::Parse(info)) {
				call.Cast<CallNode>()->node = node;
				node = call;

				continue;
			}

			if (NodePtr dot = DotParser::Parse(info)) {
				dot.Cast<DotNode>()->node = node;
				node = dot;
				continue;
			}

			if (NodePtr dot = CustomInitParser::Parse(info)) {
				dot.Cast<CustomInitNode>()->node = node;
				node = dot;
				continue;
			}

			break;
		}

		return node;
	}

	return nullptr;
}