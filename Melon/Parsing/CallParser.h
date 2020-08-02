#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class CallParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}