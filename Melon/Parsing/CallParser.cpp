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

	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;

		Pointer<CallNode> call = new CallNode(info.scope, info.GetFileInfo(info.Current(-1).line));

		while (info.Current().type != TokenType::ParenClose) {
			if (!call->args.IsEmpty()) {
				if (info.Current().type != TokenType::Comma) {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}

				info.index++;
			}

			if (info.Current().type == TokenType::Ref) {
				info.index++;
				call->attributes.Add(CallNode::ArgAttributes::Ref);
			}
			else if (info.Current().type == TokenType::NoRef) {
				info.index++;
				call->attributes.Add(CallNode::ArgAttributes::NoRef);
			}
			else {
				call->attributes.Add(CallNode::ArgAttributes::None);
			}

			if (NodePtr node = ExpressionParser::Parse(info)) {
				call->args.Add(node);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				info.index++;
			}
		}

		info.index++;

		return call;
	}

	info.index = startIndex;
	return nullptr;
}