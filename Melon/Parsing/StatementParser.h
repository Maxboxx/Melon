#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StatementParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool single = false);
			static Nodes::NodePtr ParseMultiple(ParsingInfo& info);
		};
	}
}