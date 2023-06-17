#pragma once

#include "Parser.h"

#include "Melon/Nodes/Array.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for arrays.
		class ArrayParser {
		public:
			/// Parses a character value.
			static Ptr<Nodes::Array> Parse(ParsingInfo& info);
		};
	}
}