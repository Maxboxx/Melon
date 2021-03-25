#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		///B AsParser
		/// Parses the as operator
		class AsParser {
		public:
			///T Parse
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}