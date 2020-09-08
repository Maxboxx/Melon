#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class TemplateParser {
		public:
			static Boxx::Optional<Boxx::List<Symbols::ScopeList>> Parse(ParsingInfo& info);
			static Boxx::Optional<Boxx::List<Symbols::Scope>> ParseDefine(ParsingInfo& info);
		};
	}
}