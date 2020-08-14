#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class RepeatParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}