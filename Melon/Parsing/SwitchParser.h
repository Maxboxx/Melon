#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class SwitchParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}