#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for integers.
		class IntegerParser {
		public:
			/// Parses an integer.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}