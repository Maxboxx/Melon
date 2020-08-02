#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class CustomInitParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}