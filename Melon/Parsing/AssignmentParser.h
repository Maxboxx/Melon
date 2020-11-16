#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class AssignmentParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool single = false, const bool newAssign = false);

		private:
			static Boxx::List<Symbols::ScopeList> ParseTypes(ParsingInfo& info);
		};
	}
}