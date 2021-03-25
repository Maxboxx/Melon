#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for the {as} operator.
		class AsParser {
		public:
			/// Parses an {as} operator.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}