#pragma once

#include "Boxx/Regex.h"
#include "Boxx/List.h"
#include "Boxx/Array.h"
#include "Boxx/Pair.h"
#include "Boxx/Optional.h"

#include "Token.h"
#include "Errors.h"

namespace Melon {

	///B Lexer
	/// Used to get all tokens from a code string
	class Lexer {
	public:
		///T Lex
		/// Get all from tokens from a code string
		///A pattern: A list of token patterns to search for
		///A code: The code to lex
		///A filename: The name of the file the code came from
		static Boxx::List<Token> Lex(const Boxx::List<TokenPattern>& patterns, const Boxx::String& code, const Boxx::String& filename) {
			static Boxx::Regex whiteSpace = Boxx::Regex("^%n*");
			static Boxx::Regex undefinedToken = Boxx::Regex("^~%n*");

			Boxx::String match = whiteSpace.Match(code)->match;
			Boxx::UInt line = 1 + Lines(match);
			Boxx::UInt i = match.Size();
			Boxx::List<Token> tokens;
        
			while (i < code.Size()) {
				bool found = false;
            
				for (const TokenPattern& pattern : patterns) {
					if (Boxx::Optional<Boxx::Match> match = pattern.pattern.Match(code, i)) {
						i += match->length;

						if (!pattern.ignore) {
                   			tokens.Add(Token(pattern.type, match->groups.IsEmpty() ? match->match : match->groups[0], line));
						}

						line += Lines(match->match);
					
						found = true;
						break;
					}
				}
            
				if (!found) {
					if (Boxx::Optional<Boxx::Match> match = undefinedToken.Match(code, i)) {
						ErrorLog::Error(LogMessage("error.token.undefined", match->match), FileInfo(filename, line, 0));
					}
				}
            
				if (i < code.Size()) {
					const Boxx::String match = whiteSpace.Match(code, i)->match;
					i += match.Size();
					line += Lines(match);
				}
			}

			return tokens;
		}

		static Boxx::UInt Lines(const Boxx::String& str) {
			Boxx::UInt lines = 0;

			for (const char c : str) {
				if (c == '\n') lines++;
			}

			return lines;
		}
	};
}