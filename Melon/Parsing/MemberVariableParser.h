#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for member variables.
		class MemberVariableParser {
		public:
			/// Member variable info.
			struct Variables {
				/// The types of the variables.
				Boxx::List<Symbols::NameList> types;

				/// The variable names.
				Boxx::List<Symbols::Name> names;

				~Variables() {};
			};

			/// Parses member variables.
			static Boxx::Optional<Variables> Parse(ParsingInfo& info);
		};
	}
}