#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class NewVariableParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool single = false);

		private:
			static Boxx::Optional<Symbols::ScopeList> ParseType(ParsingInfo& info);
		};
	}
}