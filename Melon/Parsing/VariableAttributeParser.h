#pragma once

#include "Boxx/Set.h"

#include "Parser.h"

namespace Melon {
	namespace Symbols {
		enum class VariableAttributes : Boxx::UByte;
	}

	namespace Parsing {
		class VariableAttributeParser {
		public:
			static Symbols::VariableAttributes Parse(ParsingInfo& info, const bool includeRef = false);
		};
	}
}