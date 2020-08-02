#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class BooleanParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}