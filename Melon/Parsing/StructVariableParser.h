#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StructVariableParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}