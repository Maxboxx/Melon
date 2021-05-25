#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for statements.
		class StatementParser {
		public:
			/// Parses a statement.
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool single = false);

			/// Parses multiple statements.
			static Nodes::NodePtr ParseMultiple(ParsingInfo& info);
		};
	}
}