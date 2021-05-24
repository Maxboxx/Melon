#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for the {.} operator
		class DotParser {
		public:
			/// Parses a {.} operation
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}