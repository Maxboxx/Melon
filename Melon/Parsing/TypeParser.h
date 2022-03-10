#pragma once

#include "Boxx/Optional.h"

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class TypeExpression;
	}

	namespace Parsing {
		/// Parser for types.
		class TypeParser {
		public:
			/// Parses a type.
			static Boxx::Optional<Symbols::NameList> Parse(ParsingInfo& info);

			/// Parses a type name.
			static Boxx::Optional<Symbols::Name> ParseName(ParsingInfo& info);
		};
	}
}