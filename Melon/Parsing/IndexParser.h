#pragma once

#include "Parser.h"

#include "Melon/Nodes/IndexExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for indexing.
		class IndexParser {
		public:
			/// Parses an index operation.
			static Ptr<Nodes::IndexExpression> Parse(ParsingInfo& info);
		};
	}
}
