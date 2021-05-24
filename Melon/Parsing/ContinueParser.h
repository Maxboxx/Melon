#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {continue} statements.
		class ContinueParser {
		public:
			/// Parses a {continue} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}