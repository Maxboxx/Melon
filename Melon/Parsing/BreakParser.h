#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class BreakParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}