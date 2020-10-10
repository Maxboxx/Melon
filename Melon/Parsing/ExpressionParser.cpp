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
#include "TypeParser.h"
#include "AsParser.h"

#include "Melon/Nodes/BinaryOperatorNode.h"
#include "Melon/Nodes/UnaryOperatorNode.h"
#include "Melon/Nodes/LogicNode.h"
#include "Melon/Nodes/CallNode.h"
#include "Melon/Nodes/DotNode.h"
#include "Melon/Nodes/CustomInitNode.h"
#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/NilNode.h"
#include "Melon/Nodes/DefaultNode.h"
#include "Melon/Nodes/SafeUnwrapNode.h"
#include "Melon/Nodes/SafeUnwrapEndNode.h"
#include "Melon/Nodes/ConvertNode.h"
#include "Melon/Nodes/IfExprNode.h"

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
				if (token.type == TokenType::As) {
					if (NodePtr node = AsParser::Parse(info)) {
						node.Cast<ConvertNode>()->node = nodes.Last();
						nodes[nodes.Size() - 1] = node;
					}
				}
				else {
					info.index++;

					if (NodePtr valueNode2 = ParseValue(info)) {
						Pointer<BinaryOperatorNode> node;

						if (IsLogic(token.type)) {
							node = new LogicNode(info.scopes, token.type, FileInfo(info.filename, token.line, info.statementNumber));
						}
						else if (token.type == TokenType::DoubleQuestion) {
							node = new DefaultNode(info.scopes, FileInfo(info.filename, token.line, info.statementNumber));
						}
						else {
							node = new BinaryOperatorNode(info.scopes, Scope(token.value), FileInfo(info.filename, token.line, info.statementNumber));
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

NodePtr ExpressionParser::ParseValue(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;
	const Token token = info.Current();

	if (NodePtr node = ParseSingleValue(info, statement)) {
		return node;
	}
	else if (token.type == TokenType::Minus || token.type == TokenType::BXor) {
		info.index++;

		if (NodePtr node = ParseValue(info)) {
			Pointer<UnaryOperatorNode> opNode = new UnaryOperatorNode(info.scopes, Scope(token.value), FileInfo(info.filename, token.line, info.statementNumber));

			opNode->node = node;
			return opNode;
		}
	}

	info.index = startIndex;
	return nullptr;
}

NodePtr ExpressionParser::ParseRawValue(ParsingInfo& info, const bool statement) {
	const UInt startIndex = info.index;
	const UInt startLine  = info.Current().line;

	if (statement) {
		ErrorLog::AddMarker();

		try {
			if (NodePtr node = IfExpressionParser::Parse(info, true)) {
				ErrorLog::RemoveMarker();
				return node;
			}
		}
		catch (CompileError e) {
			info.index = startIndex;
		}

		try {
			if (NodePtr node = SwitchExpressionParser::Parse(info, true)) {
				ErrorLog::RemoveMarker();
				return node;
			}
		}
		catch (CompileError e) {
			info.index = startIndex;
		}

		ErrorLog::Revert();
	}
	else {
		if (NodePtr node = IfExpressionParser::Parse(info)) {
			return node;
		}
		else if (NodePtr node = SwitchExpressionParser::Parse(info)) {
			return node;
		}
	}

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
	else if (info.Current().type == TokenType::Nil) {
		Pointer<NilNode> node = new NilNode(FileInfo(info.filename, info.Current().line, info.statementNumber));
		info.index++;
		return node;
	}

	if (info.Current().type == TokenType::Global) {
		const UInt line = info.Current().line;
		info.index++;
		
		if (NodePtr dotNode = DotParser::Parse(info)) {
			Pointer<DotNode> dn = dotNode.Cast<DotNode>();
			Pointer<NameNode> nn = new NameNode(ScopeList(), FileInfo(info.filename, line, info.statementNumber));
			nn->name = Scope::Global;
			dn->node = nn;
			return dn;
		}

		if (info.Current().type == TokenType::Dot) {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'.'", "'global'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("name", "'.'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		info.index = startIndex;
		return nullptr;
	}
	else if (Optional<Scope> node = TypeParser::ParseScope(info)) {
		Pointer<NameNode> nn = new NameNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber));
		nn->name = (Scope)node;
		return nn;
	}
	else if (NodePtr node = NameParser::Parse(info)) {
		return node;
	}

	info.index = startIndex;
	return nullptr;
}

NodePtr ExpressionParser::ParseSingleValue(ParsingInfo& info, const bool statement) {
	if (NodePtr node = ParseRawValue(info, statement)) {
		bool hasSafeUnwrap = false;

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

			if (info.Current().type == TokenType::Exclamation) {
				Pointer<UnaryOperatorNode> unwrap = new UnaryOperatorNode(info.scopes, Scope::Unwrap, FileInfo(info.filename, info.Current().line, info.statementNumber));
				unwrap->node = node;
				info.index++;
				node = unwrap;
				continue;
			}

			if (info.Current().type == TokenType::Question) {
				hasSafeUnwrap = true;

				Pointer<SafeUnwrapNode> unwrap = new SafeUnwrapNode(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));
				unwrap->node = node;
				info.index++;
				node = unwrap;
				continue;
			}

			break;
		}

		if (hasSafeUnwrap) {
			Pointer<SafeUnwrapEndNode> unwrap = new SafeUnwrapEndNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			unwrap->node = node;
			node = unwrap;
		}

		return node;
	}

	return nullptr;
}