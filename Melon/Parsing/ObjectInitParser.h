#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class ObjectInitParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}