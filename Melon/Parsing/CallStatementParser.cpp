#include "CallStatementParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/CallNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr CallStatementParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (NodePtr node = ExpressionParser::Parse(info, true)) {
		if (Pointer<CallNode> call = node.Cast<CallNode>()) {
			call->isStatement = true;
			info.statementNumber++;
			return call;
		}
	}

	info.index = startIndex;
	return nullptr;
}