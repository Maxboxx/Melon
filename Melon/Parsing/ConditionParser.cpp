#include "ConditionParser.h"

#include "ExpressionParser.h"
#include "AssignmentParser.h"

#include "Melon/Nodes/Assignment.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Condition> ConditionParser::Parse(ParsingInfo& info, const bool includeAssign) {
	if (includeAssign) {
		if (Ptr<Assignment> assign = AssignmentParser::Parse(info, AssignmentParser::Flags::Single | AssignmentParser::Flags::MethodCall)) {
			Ptr<Condition> cond = new Condition(info.scope, assign->File());
			cond->assign = assign;
			return cond;
		}
	}

	if (Ptr<Expression> node = ExpressionParser::Parse(info)) {
		Ptr<Condition> cond = new Condition(info.scope, node->File());
		cond->expression = node;
		return cond;
	}

	return nullptr;
}