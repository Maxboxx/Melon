#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class AssignableParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}