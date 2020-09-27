#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class AsParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}