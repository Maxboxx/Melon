#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for call statements.
		class CallStatementParser {
		public:
			/// Parses a call statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}