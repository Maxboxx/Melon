#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {repeat} statements.
		class RepeatParser {
		public:
			/// Parses a {repeat} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}
