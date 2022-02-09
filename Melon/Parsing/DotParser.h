#pragma once

#include "Parser.h"

#include "Melon/Nodes/DotExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for the {.} operator.
		class DotParser {
		public:
			/// Parses a {.} expression.
			static Ptr<Nodes::DotExpression> Parse(ParsingInfo& info);
		};
	}
}