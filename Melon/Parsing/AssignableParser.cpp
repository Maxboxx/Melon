#include "AssignableParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/DotNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr AssignableParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (NodePtr node = ExpressionParser::Parse(info, true)) {
		if (node.Cast<NameNode>()) return node;
		if (node.Cast<DotNode>())  return node;

		info.index = startIndex;
		return nullptr;
	}

	return nullptr;
}