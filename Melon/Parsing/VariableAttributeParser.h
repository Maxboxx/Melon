#pragma once

#include "Boxx/Set.h"

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Symbols {
		enum class VariableAttributes : Boxx::UByte;
	}

	namespace Parsing {
		/// Parser for variable attributes.
		class VariableAttributeParser {
		public:
			/// Parses variable attributes.
			static Symbols::VariableAttributes Parse(ParsingInfo& info, const bool includeRef = false);
		};
	}
}