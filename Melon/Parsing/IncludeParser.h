#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for including files.
		class IncludeParser {
		public:
			/// Parses an {include}.
			///R {true} if the parsing was successful.
			static bool Parse(ParsingInfo& info);

			/// Parses a file to include to the project.
			static void Include(const Symbols::NameList& include, ParsingInfo& info);

		private:
			static void IncludeNamespace(const Boxx::String& dir, const Symbols::NameList& include, Boxx::UInt index, ParsingInfo& info);

			static void ParseFile(const Boxx::String& filename, const Symbols::NameList& include, const Symbols::Name& includeFile, ParsingInfo& info);
			static void CreateIncludeSymbols(const Boxx::String& filename, const Symbols::NameList& include);
			static void ParseDirectory(const Boxx::String& directory, const Symbols::NameList& include, ParsingInfo& options);
		};
	}
}