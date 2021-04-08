#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {do} statements.
		class DoParser {
		public:
			/// Parses a {do} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}
