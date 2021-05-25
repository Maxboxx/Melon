#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for new variables.
		class NewVariableParser {
		public:
			/// Parses a new variables.
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool single = false);

		private:
			static Boxx::Optional<Symbols::NameList> ParseType(ParsingInfo& info);
		};
	}
}