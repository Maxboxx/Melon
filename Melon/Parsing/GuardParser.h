#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {guard} statements.
		class GuardParser {
		public:
			/// Parses a {guard} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}