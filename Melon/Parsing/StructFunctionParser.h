#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StructFunctionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}