#pragma once

#include "Parser.h"

#include "Melon/Nodes/RepeatStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {repeat} statements.
		class RepeatParser {
		public:
			/// Parses a {repeat} statement.
			static Ptr<Nodes::RepeatStatement> Parse(ParsingInfo& info);
		};
	}
}
