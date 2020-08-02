#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class AssignmentParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}