#include "CallParser.h"

#include "ExpressionParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<CallExpression> CallParser::Parse(ParsingInfo& info) {
	if (info.EndOfFile()) return nullptr;

	const UInt startIndex = info.index;

	// Parse argument list
	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;

		Ptr<CallExpression> call = new CallExpression(info.scope, info.GetFileInfoPrev());

		// Parse arguments
		while (info.Current().type != TokenType::ParenClose) {
			if (!call->arguments.IsEmpty()) {
				if (info.Current().type != TokenType::Comma) {
					ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(")"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				}

				info.index++;
			}

			call->modifiers.Add(ParseArgumentAttributes(info));

			if (Ptr<Expression> node = ExpressionParser::Parse(info)) {
				call->arguments.Add(node);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(")"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				info.index++;
			}
		}

		info.index++;

		return call;
	}

	info.index = startIndex;
	return nullptr;
}

Ptr<CallStatement> CallParser::ParseStatement(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (Ptr<Expression> node = ExpressionParser::Parse(info, true)) {
		if (Ptr<CallExpression> callExpr = node.AsPtr<CallExpression>()) {
			info.statementNumber++;

			Ptr<CallStatement> call = new CallStatement(callExpr->scope, callExpr->File());
			call->arguments  = callExpr->arguments;
			call->modifiers = callExpr->modifiers;
			call->expression = callExpr->expression;
			return call;
		}
	}

	info.index = startIndex;
	return nullptr;
}

CallArgAttributes CallParser::ParseArgumentAttributes(ParsingInfo& info) {
	switch (info.Current().type) {
		case TokenType::Ref: {
			info.index++;
			return CallArgAttributes::Ref;
		}

		case TokenType::NoRef: {
			info.index++;
			return CallArgAttributes::NoRef;
		}

		default: {
			return CallArgAttributes::None;
		}
	}
}
