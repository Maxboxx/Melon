#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class ContinueParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}