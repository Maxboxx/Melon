#pragma once

#include "Types.h"
#include "Token.h"
#include "Regex.h"
#include "List.h"
#include "Array.h"
#include "Error.h"
#include "String.h"

///N Lexer

namespace Boxx {

	///B Lexer
	/// Used to get all tokens from a string
	class Lexer {
	public:
		///T Lex
		/// Get all from tokens from a code string
		///A const List<TokenPattern<T>>& patterns: A list of token patterns to search for
		///A const String& code: The code to lex
		///E LexerError: Thrown if the string contains an undefined token
		///M
		template <class T>
		static TokenList<T> Lex(const List<TokenPattern<T>>& patterns, const String& code);
		///M
		
	private:
		static UInt Lines(const String& str) {
			UInt lines = 0;

			for (const char c : str) {
				if (c == '\n') lines++;
			}

			return lines;
		}
	};

	///B LexerError
	/// Thrown if the lexer can not lex a string
	class LexerError : public Error {
	public:
		LexerError() : Error() {}
		LexerError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "LexerError";
		}
	};

	template <class T>
	static TokenList<T> Lexer::Lex(const List<TokenPattern<T>>& patterns, const String& code) {
		static Regex whiteSpace = Regex("^%n*");
		static Regex undefinedToken = Regex("^~%n*");

		if (code.Size() == 0) return TokenList<T>();

		String match = whiteSpace.Match(code)->match;
		UInt line = 1 + Lines(match);
		UInt i = match.Size();
		List<Token<T>> tokens;

		while (i < code.Size()) {
			bool found = false;

			for (const TokenPattern<T>& pattern : patterns) {
				Optional<Match> match = pattern.pattern.Match(code, i);

				if (match) {
					Match m = *match;

					i += m.length;

					if (!pattern.ignore) {
						tokens.Add(Token<T>(pattern.type, m.groups.IsEmpty() ? m.match : m.groups[0], m.match, line));
					}

					line += Lines(m.match);

					found = true;
					break;
				}
			}

			if (!found) {
				Optional<Match> match = undefinedToken.Match(code, i);

				if (match) {
					throw LexerError("Undefined token '" + match->match + "'");
				}
			}

			if (i < code.Size()) {
				const String match = whiteSpace.Match(code, i)->match;
				i += match.Size();
				line += Lines(match);
			}
		}

		return TokenList<T>(tokens);
	}
}
