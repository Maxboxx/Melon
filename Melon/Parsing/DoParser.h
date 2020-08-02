#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class DoParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}