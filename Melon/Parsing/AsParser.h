#pragma once

#include "Parser.h"

#include "Melon/Nodes/TypeConversion.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for the {as} operator.
		class AsParser {
		public:
			/// Parses an {as} operator.
			static Ptr<Nodes::TypeConversion> Parse(ParsingInfo& info);
		};
	}
}