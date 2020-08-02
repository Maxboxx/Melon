#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class ReturnParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}