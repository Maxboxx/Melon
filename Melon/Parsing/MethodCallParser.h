#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class MethodCallParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}