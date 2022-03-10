#pragma once

#include "Parser.h"

#include "Melon/Nodes/NameExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for names.
		class NameParser {
		public:
			/// Parses a name.
			static Ptr<Nodes::NameExpression> Parse(ParsingInfo& info);
		};
	}
}