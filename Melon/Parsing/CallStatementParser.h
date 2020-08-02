#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class CallStatementParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}