#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {return} statements.
		class ReturnParser {
		public:
			/// Parses a {return} statement.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}