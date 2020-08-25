#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class GuardParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}