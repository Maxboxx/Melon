#pragma once

#include "Boxx/Optional.h"

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class TypeParser {
		public:
			static Boxx::Optional<Symbols::ScopeList> Parse(ParsingInfo& info);
		};
	}
}