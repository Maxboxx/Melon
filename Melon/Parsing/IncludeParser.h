#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class IncludeParser {
		public:
			static bool Parse(ParsingInfo& info);

			///T Parse Include
			/// Parses a file to include to the project
			static void ParseInclude(const Symbols::NameList& include, ParsingInfo& info);

		private:
			static void ParseFile(const Boxx::String& filename, const Symbols::NameList& include, const Symbols::Name& includeFile, ParsingInfo& info);
			static void CreateIncludeSymbols(const Boxx::String& filename, const Symbols::NameList& include);
			static void ParseDirectory(const Boxx::String& directory, const Symbols::NameList& include, ParsingInfo& options);
		};
	}
}