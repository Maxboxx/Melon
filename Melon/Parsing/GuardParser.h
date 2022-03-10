#pragma once

#include "Parser.h"

#include "Melon/Nodes/GuardStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {guard} statements.
		class GuardParser {
		public:
			/// Parses a {guard} statement.
			static Ptr<Nodes::GuardStatement> Parse(ParsingInfo& info);
		};
	}
}