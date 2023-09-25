#pragma once

#include "Parser.h"

#include "Melon/Nodes/StringLiteral.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for strings values.
		class StringParser {
		public:
			/// Parses a string.
			static Ptr<Nodes::StringLiteral> Parse(ParsingInfo& info);
		};
	}
}