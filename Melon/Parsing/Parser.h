#pragma once

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
		///B Parser
		/// Used to parse a list of tokens
		class Parser {
		public:
			///T Parse
			/// Parses a file and converts it to an AST
			static ParsingInfo Parse(const Boxx::String& filename, const CompilerOptions& options);

			///T Unexpected token
			/// Throws a <type>SyntaxError</type> for the current token
			static Nodes::NodePtr UnexpectedToken(ParsingInfo& info);

		private:
			friend class IncludeParser;

			static void ParseFile(const Boxx::String& filename, ParsingInfo& info);
			static void SetupTokens();
			static Boxx::List<TokenPattern> GetTokenPatterns();

			static Boxx::List<TokenPattern> patterns;
		};
	}
}