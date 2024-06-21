#pragma once

#include "Boxx/List.h"
#include "Boxx/String.h"
#include "Boxx/Set.h"

#include "Melon/Token.h"
#include "Melon/FileInfo.h"
#include "Melon/MelonCompiler.h"

#include "Melon/Symbols/Symbol.h"
#include "Melon/Symbols/MapSymbol.h"
#include "Melon/Symbols/NameList.h"

#include "Melon/Nodes/RootNode.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Information used by the parser.
		struct ParsingInfo {
			/// The root node of the AST.
			Nodes::RootNode root;

			/// The current file that is being parsed.
			Boxx::String filename;

			/// The include name of the current file.
			Symbols::Name currentFile;

			/// The current namespace.
			Symbols::NameList currentNamespace;

			/// The tokens to parse.
			Boxx::List<Token> tokens;

			/// The current token in the token list.
			Boxx::UInt index;

			/// The number of loops the parser is currently inside of.
			Boxx::UInt loops;

			/// The number of breakable scopes the parser is currently inside of.
			Boxx::UInt scopeCount;

			/// Options for the compiler.
			CompilerOptions options;

			/// All files included by the project.
			Boxx::Set<Boxx::String> includedFiles;

			/// All namespaces included by the current file.
			Boxx::Set<Symbols::NameList> includedNamespaces;

			/// All namespaces imported by the current file.
			Boxx::Set<Symbols::NameList> importedNamespaces;

			/// The current statement number.
			Boxx::UInt statementNumber = 1;

			/// The symbol for the current scope.
			Symbols::MapSymbol* scope;

			/// Advances to the next token and returns it.
			Token Next();

			/// Advances {steps} steps and returns the token at that position.
			Token Next(const Boxx::UInt steps);

			/// Returns the next token.
			Token Peek() const;

			/// Returns the token {steps} steps ahead. 
			Token Peek(const Boxx::UInt steps) const;

			/// Returns the previous token.
			Token Prev() const;

			/// Returns the token {steps} steps back.
			Token Prev(const Boxx::UInt steps) const;

			/// {true} if the end of file has been reached.
			bool EndOfFile() const;

			/// Returns the current token.
			Token Current() const;

			/// Gets the file info for the current token.
			FileInfo GetFileInfo() const;

			/// Gets the file info for the specified line.
			FileInfo GetFileInfo(const Boxx::UInt line) const;

			/// Gets the file info for the previous token.
			FileInfo GetFileInfoPrev() const;

			/// Gets the file info for the token {steps} steps back.
			FileInfo GetFileInfoPrev(const Boxx::UInt steps) const;

			/// Gets the file info for the next token.
			FileInfo GetFileInfoNext() const;

			/// Gets the file info for the token {steps} steps ahead.
			FileInfo GetFileInfoNext(const Boxx::UInt steps) const;
		};
	}
}
