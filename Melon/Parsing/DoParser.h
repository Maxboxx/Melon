#pragma once

#include "Parser.h"

#include "Melon/Nodes/DoStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {do} statements.
		class DoParser {
		public:
			/// Parses a {do} statement.
			static Ptr<Nodes::DoStatement> Parse(ParsingInfo& info);
		};
	}
}
