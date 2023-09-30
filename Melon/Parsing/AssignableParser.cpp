#include "AssignableParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/NameExpression.h"
#include "Melon/Nodes/DotExpression.h"
#include "Melon/Nodes/DiscardExpression.h"
#include "Melon/Nodes/IndexExpression.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Expression> AssignableParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	// Parse discard
	if (info.Current().type == TokenType::Discard) {
		info.index++;
		return new DiscardExpression(info.scope, info.GetFileInfoPrev());
	}
	// Parse expression
	else if (Ptr<Expression> node = ExpressionParser::Parse(info, true)) {
		if (node.Is<NameExpression>())  return node;
		if (node.Is<DotExpression>())   return node;
		if (node.Is<IndexExpression>()) return node;

		info.index = startIndex;
		return nullptr;
	}

	return nullptr;
}