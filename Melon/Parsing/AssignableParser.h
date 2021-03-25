#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for assignable expressions.
		class AssignableParser {
		public:
			/// Parses an assignable expression.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}