#pragma once

#include "Parser.h"

#include "Melon/Nodes/Character.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for character values.
		class CharParser {
		public:
			/// Parses a character value.
			static Ptr<Nodes::Character> Parse(ParsingInfo& info);
		};
	}
}