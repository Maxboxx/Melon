#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for conditional expressions.
		class ConditionParser {
		public:
			/// Parses a conditional expression.
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool includeAssign = true);
		};
	}
}