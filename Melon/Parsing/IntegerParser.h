#pragma once

#include "Parser.h"

#include "Melon/Nodes/Integer.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for integers.
		class IntegerParser {
		public:
			/// Parses an integer.
			static Ptr<Nodes::Integer> Parse(ParsingInfo& info);
		};
	}
}