#pragma once

#include "Parser.h"

#include "Melon/Nodes/IsExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for the {is} operator.
		class IsParser {
		public:
			/// Parses an {is} operator.
			static Ptr<Nodes::IsExpression> Parse(ParsingInfo& info);
		};
	}
}