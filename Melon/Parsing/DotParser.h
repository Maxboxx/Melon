#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class DotParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}