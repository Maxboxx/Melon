#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for templates.
		class TemplateParser {
		public:
			/// Parses a template argument list.
			static Boxx::Optional<Boxx::List<Symbols::NameList>> Parse(ParsingInfo& info);

			/// Parses a template definition.
			static Boxx::Optional<Boxx::List<Symbols::NameList>> ParseDefine(ParsingInfo& info);
		};
	}
}