#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for names.
		class NameParser {
		public:
			/// Parses a name.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}