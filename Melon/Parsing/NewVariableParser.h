#pragma once

#include "Parser.h"

#include "Melon/Nodes/NewVariableNode.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for new variables.
		class [[deprecated]] NewVariableParser {
		public:
			/// Parses a new variables.
			static Ptr<Nodes::NewVariableNode> Parse(ParsingInfo& info, const bool single = false);

		private:
			static Boxx::Optional<Symbols::NameList> ParseType(ParsingInfo& info);
		};
	}
}