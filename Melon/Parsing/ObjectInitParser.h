#pragma once

#include "Parser.h"

#include "Melon/Nodes/ObjectInitExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for object initializers.
		class ObjectInitParser {
		public:
			/// Parses an object initializer.
			static Ptr<Nodes::ObjectInitExpression> Parse(ParsingInfo& info);
		};
	}
}