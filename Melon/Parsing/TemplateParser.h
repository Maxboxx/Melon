#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class TemplateParser {
		public:
			static Boxx::Optional<Boxx::List<Symbols::NameList>> Parse(ParsingInfo& info);
			static Boxx::Optional<Boxx::List<Symbols::NameList>> ParseDefine(ParsingInfo& info);
		};
	}
}