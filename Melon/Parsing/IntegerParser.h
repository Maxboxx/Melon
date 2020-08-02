#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class IntegerParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}