#include "CallStatementParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/CallNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr CallStatementParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (NodePtr node = ExpressionParser::Parse(info, true)) {
		if (node.Cast<CallNode>()) {
			return node;
		}
	}

	info.index = startIndex;
	return nullptr;
}