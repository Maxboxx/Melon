#pragma once

#include "Boxx/Optional.h"

#include "Parser.h"

namespace Melon {
	namespace Nodes {
		class TypeNode;
	}

	namespace Parsing {
		class TypeParser {
		public:
			static Boxx::Optional<Symbols::NameList> Parse(ParsingInfo& info);
			static Boxx::Optional<Symbols::Name> ParseScope(ParsingInfo& info);
		};
	}
}