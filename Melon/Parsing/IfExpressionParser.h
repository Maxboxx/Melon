#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class IfExpressionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool returnOnError = false);
		};
	}
}