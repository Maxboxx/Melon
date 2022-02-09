#pragma once

#include "Parser.h"

#include "Melon/Nodes/ReturnStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {return} statements.
		class ReturnParser {
		public:
			/// Parses a {return} statement.
			static Ptr<Nodes::ReturnStatement> Parse(ParsingInfo& info);
		};
	}
}