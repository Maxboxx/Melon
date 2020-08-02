#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class NewVariableParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}