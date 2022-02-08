#pragma once

#include "Parser.h"

#include "Melon/Nodes/Expression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for assignable expressions.
		class AssignableParser {
		public:
			/// Parses an assignable expression.
			static Ptr<Nodes::Expression> Parse(ParsingInfo& info);
		};
	}
}
