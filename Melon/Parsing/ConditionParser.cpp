#include "ConditionParser.h"

#include "ExpressionParser.h"
#include "AssignmentParser.h"

#include "Melon/Nodes/ConditionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr ConditionParser::Parse(ParsingInfo& info, const bool includeAssign) {
	Pointer<ConditionNode> cond = nullptr;

	if (includeAssign) {
		if (NodePtr node = AssignmentParser::Parse(info, true)) {
			cond = new ConditionNode(info.scopes, node->file);
			cond->cond = node;
			return cond;
		}
	}

	if (NodePtr node = ExpressionParser::Parse(info)) {
		cond = new ConditionNode(info.scopes, node->file);
		cond->cond = node;
		return cond;
	}

	return nullptr;
}