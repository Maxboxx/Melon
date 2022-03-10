#pragma once

#include "Types.h"

#include "Boxx/List.h"
#include "Boxx/Pointer.h"
#include "Boxx/String.h"

#include "Melon/Token.h"
#include "Melon/MelonCompiler.h"

#include "Melon/Nodes/Node.h"

#include "ParsingInfo.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for melon files.
		class Parser {
		public:
			/// Parses a melon file.
			static ParsingInfo Parse(const Boxx::String& filename, const CompilerOptions& options);

			/// Generates an unexpected token error.
			static void UnexpectedToken(ParsingInfo& info);

		private:
			friend class IncludeParser;

			static void ParseFile(const Boxx::String& filename, ParsingInfo& info);
			static void SetupTokens();
			static Boxx::List<TokenPattern> GetTokenPatterns();

			static Boxx::List<TokenPattern> patterns;
		};
	}
}