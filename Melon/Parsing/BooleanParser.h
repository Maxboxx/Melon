#pragma once

#include "Parser.h"

#include "Melon/Nodes/Boolean.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for boolean values.
		class BooleanParser {
		public:
			/// Parses a boolean value.
			static Ptr<Nodes::Boolean> Parse(ParsingInfo& info);
		};
	}
}