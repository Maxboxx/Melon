#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class ConditionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool includeAssign = true);
		};
	}
}