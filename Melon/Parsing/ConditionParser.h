#pragma once

#include "Parser.h"

#include "Melon/Nodes/Condition.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for conditional expressions.
		class ConditionParser {
		public:
			/// Parses a conditional expression.
			static Ptr<Nodes::Condition> Parse(ParsingInfo& info, const bool includeAssign = true);
		};
	}
}