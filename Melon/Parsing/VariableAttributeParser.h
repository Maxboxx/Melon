#pragma once

#include "Boxx/Set.h"

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class VariableAttributeParser {
		public:
			static Boxx::Set<Symbols::SymbolAttribute> Parse(ParsingInfo& info, const bool includeRef = false);
		};
	}
}