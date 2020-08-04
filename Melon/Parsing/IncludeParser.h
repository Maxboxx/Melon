#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class IncludeParser {
		public:
			static bool Parse(ParsingInfo& info);

			///T Parse Include
			/// Parses a file to include to the project
			static void ParseInclude(const Symbols::ScopeList& include, ParsingInfo& info);

		private:
			static void ParseFile(const Boxx::String& filename, const Symbols::ScopeList& include, const Symbols::Scope& includeFile, ParsingInfo& info);
			static void CreateIncludeSymbols(const Boxx::String& filename, const Symbols::ScopeList& include);
			static void ParseDirectory(const Boxx::String& directory, const Symbols::ScopeList& include, ParsingInfo& options);
		};
	}
}