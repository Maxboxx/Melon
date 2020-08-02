#pragma once

#include "Boxx/List.h"
#include "Boxx/Pointer.h"
#include "Boxx/String.h"
#include "Boxx/Set.h"

#include "Melon/Token.h"
#include "Melon/Errors.h"
#include "Melon/MelonCompiler.h"

#include "Melon/Symbols/Symbols.h"
#include "Melon/Symbols/ScopeList.h"

#include "Melon/Nodes/Node.h"
#include "Melon/Nodes/RootNode.h"

namespace Melon {
	namespace Parsing {

		///B ParsingInfo
		struct ParsingInfo {
			///T Root node
			/// The root node of the AST
			Nodes::RootNode root;

			///T Filename
			Boxx::String filename;

			///T Namespace
			Symbols::ScopeList currentNamespace;

			///T Scopes
			/// Used to keep track of the current scope
			Symbols::ScopeList scopes;

			///T Tokens
			/// The tokens to parse
			Boxx::List<Token> tokens;

			///T Token index
			/// The current token in the token list
			Boxx::UInt index;

			///T Loop counter
			/// The amount of loops the parser is currently inside of
			Boxx::UInt loops;

			///T CompilerOptions
			CompilerOptions options;

			///T Included files
			/// All files included by the project
			Boxx::Set<Boxx::String> includedFiles;

			///T Included namespaces
			/// All namespaces included by the current file
			Boxx::Set<Symbols::ScopeList> includedNamespaces;

			///H Methods

			///T Next
			/// Advances to the next token and returns it
			Token Next();

			///T Previous
			/// Goes back to the previous token and returns it
			Token Previous();

			///T End of file
			/// Checks if the end of file has been reached
			bool EndOfFile() const;

			///T Current
			///A offset: The offset to grab the token from
			///R Token: The current token
			///M
			Token Current();
			Token Current(const Boxx::Int offset);
			///M
		};

		///B Parser
		/// Used to parse a list of tokens
		class Parser {
		public:
			///T Parse
			/// Parses a file and converts it to an AST
			static Nodes::RootNode Parse(const Boxx::String& filename, const CompilerOptions& options);

			///T Unexpected token
			/// Throws a <type>SyntaxError</type> for the current token
			static Nodes::NodePtr UnexpectedToken(ParsingInfo& info);
		private:
			friend class IncludeParser;

			static void ParseFile(const Boxx::String& filename, ParsingInfo& info);
			static Nodes::NodePtr ParseNext(ParsingInfo& info);
			static void SetupTokens();
			static Boxx::List<TokenPattern> GetTokenPatterns();

			static Boxx::List<TokenPattern> patterns;
		};
	}
}