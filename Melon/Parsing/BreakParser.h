#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {break}, {abort}, and {break!} statements.
		class BreakParser {
		public:
			/// Parses a {break}, {abort}, and {break!} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static Nodes::NodePtr ParseBreak(ParsingInfo& info);
			static Nodes::NodePtr ParseAbort(ParsingInfo& info);
		};
	}
}