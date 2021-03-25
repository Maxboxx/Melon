#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for boolean values.
		class BooleanParser {
		public:
			/// Parses a boolean value.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}