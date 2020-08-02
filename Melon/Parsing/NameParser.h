#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class NameParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}