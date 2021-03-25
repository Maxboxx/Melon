#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		///B AssignableParser
		/// Parses an assignable expression
		class AssignableParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}