#pragma once

#include "Parser.h"

#include "Melon/Nodes/ContinueStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {continue} statements.
		class ContinueParser {
		public:
			/// Parses a {continue} statement.
			static Ptr<Nodes::ContinueStatement> Parse(ParsingInfo& info);
		};
	}
}