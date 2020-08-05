#pragma once

#include "Boxx/Regex.h"
#include "Boxx/List.h"
#include "Boxx/Array.h"
#include "Boxx/Pair.h"

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

			Boxx::String match = whiteSpace.Match(code)[0];
			Boxx::UInt line = 1 + Lines(match);
			Boxx::UInt i = match.Size();
			Boxx::List<Token> tokens;
        
			while (i < code.Size()) {
				bool found = false;
            
				for (const TokenPattern& pattern : patterns) {
					Boxx::Array<Boxx::String> match = pattern.pattern.Match(code, i);

					if (!match.IsEmpty()) {
						i += match[0].Size();

						if (!pattern.ignore) {
                   			tokens.Add(Token(pattern.type, match[0], line));
						}

						line += Lines(match[0]);
					
						found = true;
						break;
					}
				}
            
				if (!found) {
					Boxx::Array<Boxx::String> match = undefinedToken.Match(code, i);

					if (!match.IsEmpty()) {
						ErrorLog::Error(TokenError(TokenError::UndefinedStart + match[0] + TokenError::UndefinedEnd, FileInfo(filename, line, 0)));
					}
				}
            
				if (i < code.Size()) {
					const Boxx::String match = whiteSpace.Match(code, i)[0];
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