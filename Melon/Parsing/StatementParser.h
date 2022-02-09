#pragma once

#include "Parser.h"

#include "Melon/Nodes/Statement.h"
#include "Melon/Nodes/Statements.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for statements.
		class StatementParser {
		public:
			/// Parses a statement.
			static Ptr<Nodes::Statement> Parse(ParsingInfo& info, const bool single = false);

			/// Parses multiple statements.
			static Ptr<Nodes::Statements> ParseMultiple(ParsingInfo& info);
		};
	}
}