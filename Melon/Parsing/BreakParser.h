#pragma once

#include "Parser.h"

#include "Melon/Nodes/BreakStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {break}, {abort}, and {break!} statements.
		class BreakParser {
		public:
			/// Parses a {break}, {abort}, and {break!} statement.
			static Ptr<Nodes::BreakStatement> Parse(ParsingInfo& info);

		private:
			static Ptr<Nodes::BreakStatement> ParseBreak(ParsingInfo& info);
			static Ptr<Nodes::BreakStatement> ParseAbort(ParsingInfo& info);
		};
	}
}