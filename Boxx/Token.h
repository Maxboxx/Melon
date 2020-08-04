#pragma once

#include "Types.h"
#include "String.h"
#include "Regex.h"

namespace Boxx {
	///B Token
	/// Contains info about a token
	///M
	template <class T>
	struct Token {
	///M
		///T Type
		/// The type of the token
		T type;

		///T Value
		/// Contains the matched string for the token
		String value;

		///T Line
		/// The line number for the token
		UInt line;

		Token() {}

		///T Constructor
		Token(const T type, const String& value, const UInt line = 0) {
			this->type = type;
			this->value = value;
			this->line = line;
		}
	};

	///B TokenPattern
	/// Contains the pattern for a <code>Token</code>
	///M
	template <class T>
	struct TokenPattern {
	///M
		T type;
		Boxx::Regex pattern;
		bool ignore;

		TokenPattern() {}

		///T Constructor
		///A const T type: The token type for the pattern
		///A const String& pattern: The regex pattern for the token
		///A const bool ignoreBoundary: Ignores word boundary at the end of the patterns. Used for operators
		///A const bool ignore: Deletes token after lexing it
		TokenPattern(const T type, const String& pattern, const bool ignoreBoundary = false, const bool ignore = false) {
			this->type = type;
			this->ignore = ignore;

			if (ignoreBoundary) {
				this->pattern = Regex("^(" + pattern + ")");
			}
			else {
				this->pattern = Regex("^(" + pattern + ")%b");
			}
		}
	};
}