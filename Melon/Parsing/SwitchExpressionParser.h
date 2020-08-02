#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class SwitchExpressionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool returnOnError = false);
		};
	}
}