#include "CallParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/CallNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr CallParser::Parse(ParsingInfo& info) {
	if (info.EndOfFile()) return nullptr;

	const UInt startIndex = info.index;

	// Parse argument list
	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;

		Pointer<CallNode> call = new CallNode(info.scope, info.GetFileInfoPrev());

		// Parse arguments
		while (info.Current().type != TokenType::ParenClose) {
			if (!call->arguments.IsEmpty()) {
				if (info.Current().type != TokenType::Comma) {
					ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(")"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				}

				info.index++;
			}

			call->attributes.Add(ParseArgumentAttributes(info));

			if (NodePtr node = ExpressionParser::Parse(info)) {
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

CallNode::ArgAttributes CallParser::ParseArgumentAttributes(ParsingInfo& info) {
	switch (info.Current().type) {
		case TokenType::Ref: {
			info.index++;
			return CallNode::ArgAttributes::Ref;
		}

		case TokenType::NoRef: {
			info.index++;
			return CallNode::ArgAttributes::NoRef;
		}

		default: {
			return CallNode::ArgAttributes::None;
		}
	}
}
