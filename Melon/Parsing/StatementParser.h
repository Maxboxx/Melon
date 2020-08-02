#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StatementParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
			static Nodes::NodePtr ParseMultiple(ParsingInfo& info);
		};
	}
}