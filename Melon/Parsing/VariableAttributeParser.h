#pragma once

#include "Boxx/Set.h"

#include "Parser.h"

namespace Melon {
	namespace Symbols {
		enum class VariableAttributes;
	}

	namespace Parsing {
		class VariableAttributeParser {
		public:
			static VariableAttributes Parse(ParsingInfo& info, const bool includeRef = false);
		};
	}
}