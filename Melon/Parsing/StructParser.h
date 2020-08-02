#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StructParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}