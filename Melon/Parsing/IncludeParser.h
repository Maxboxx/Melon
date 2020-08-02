#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class IncludeParser {
		public:
			static bool Parse(ParsingInfo& info);

		private:
			static void ParseFile(const Boxx::String& filename, const Symbols::ScopeList& include, ParsingInfo& info);
		};
	}
}