#include "AssignableParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/DotNode.h"
#include "Melon/Nodes/DiscardNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr AssignableParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	// Parse discard
	if (info.Current().type == TokenType::Discard) {
		info.index++;
		return new DiscardExpression(info.scope, info.GetFileInfoPrev());
	}
	// Parse expression
	else if (NodePtr node = ExpressionParser::Parse(info, true)) {
		if (node.Cast<NameExpression>()) return node;
		if (node.Cast<DotExpression>())  return node;

		info.index = startIndex;
		return nullptr;
	}

	return nullptr;
}