#pragma once

#include "Types.h"
#include "String.h"
#include "List.h"
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

		///T Raw Value
		/// Contains the raw matched string for the token
		String rawValue;

		///T Line
		/// The line number for the token
		UInt line;

		Token() {}

		///T Constructor
		Token(const T type, const String& value, const String& rawValue, const UInt line = 0) {
			this->type = type;
			this->value = value;
			this->rawValue = rawValue;
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
				this->pattern = Regex("^{" + pattern + "}");
			}
			else {
				this->pattern = Regex("^{" + pattern + "}%b");
			}
		}
	};

	///B TokenList
	/// A list of tokens used for parsing
	///M
	template <class T>
	class TokenList {
	///M
	public:
		TokenList();
		explicit TokenList(const List<Token<T>>& list);
		TokenList(const TokenList<T>& list);
		TokenList(TokenList<T>&& list) noexcept;

		~TokenList();

		///H Methods

		///T Current
		/// Gets the current token
		Token<T> Current() const;

		///T Advance
		/// Advances to the next token
		///M
		void Advance();
		void Advance(const UInt steps);
		///M

		///T Next
		/// Advances to the next token and returns it
		Token<T> Next();

		///T Peek Next
		/// Get the next token without advancing
		///M
		Token<T> PeekNext() const;
		Token<T> PeekNext(const UInt steps) const;
		///M

		///T Peek Previous
		/// Gets the previous token
		///M
		Token<T> PeekPrevious() const;
		Token<T> PeekPrevious(const UInt steps) const;
		///M

		///T Get Pos
		/// Gets the current position in the token list
		UInt GetPos() const;

		///T Set Pos
		/// Sets the position in the token list
		void SetPos(const UInt pos);

		///T At End
		/// Checks if the current position is after the last token
		///M
		bool AtEnd() const;
		bool AtEnd(const UInt steps) const;
		///M

		///T Size
		/// Gets the size of the token list
		UInt Size() const;
		
		void operator=(const TokenList<T>& list);
		void operator=(TokenList<T>&& list) noexcept;

	private:
		List<Token<T>> list;
		UInt pos = 0;
	};

	///B TokenListError
	/// Thrown for token list errors
	class TokenListError : public Error {
	public:
		TokenListError() : Error() {}
		TokenListError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "TokenListError";
		}
	};

	template <class T>
	inline TokenList<T>::TokenList() {

	}

	template <class T>
	inline TokenList<T>::TokenList(const List<Token<T>>& list) {
		this->list = list;
	}

	template <class T>
	inline TokenList<T>::TokenList(const TokenList<T>& list) {
		this->list = list.list;
		this->pos = list.pos;
	}

	template <class T>
	inline TokenList<T>::TokenList(TokenList<T>&& list) noexcept {
		this->list = std::move(list.list);
		this->pos = list.pos;
	}

	template <class T>
	inline TokenList<T>::~TokenList() {
		
	}

	template <class T>
	inline Token<T> TokenList<T>::Current() const {
		if (pos >= Size()) throw TokenListError("Token position out of range");
		return list[pos];
	}

	template <class T>
	inline void TokenList<T>::Advance() {
		pos++;
	}

	template <class T>
	inline void TokenList<T>::Advance(const UInt steps) {
		pos += steps;
	}

	template <class T>
	inline Token<T> TokenList<T>::Next() {
		Advance();
		return Current();
	}

	template <class T>
	inline Token<T> TokenList<T>::PeekNext() const {
		if (pos + 1 >= Size()) throw TokenListError("Token position out of range");
		return list[pos + 1];
	}

	template <class T>
	inline Token<T> TokenList<T>::PeekNext(const UInt steps) const {
		if (pos + steps >= Size()) throw TokenListError("Token position out of range");
		return list[pos + steps];
	}

	template <class T>
	inline Token<T> TokenList<T>::PeekPrevious() const {
		if ((Long)pos - 1 < 0) throw TokenListError("Token position out of range");
		return list[pos - 1];
	}

	template <class T>
	inline Token<T> TokenList<T>::PeekPrevious(const UInt steps) const {
		if ((Long)pos - steps) throw TokenListError("Token position out of range");
		return list[pos - steps];
	}

	template <class T>
	inline UInt TokenList<T>::GetPos() const {
		return pos;
	}

	template <class T>
	inline void TokenList<T>::SetPos(const UInt pos) {
		this->pos = pos;
	}

	template <class T>
	inline bool TokenList<T>::AtEnd() const {
		return pos >= Size();
	}

	template <class T>
	inline bool TokenList<T>::AtEnd(const UInt steps) const {
		return pos + steps >= Size();
	}

	template <class T>
	inline UInt TokenList<T>::Size() const {
		return list.Size();
	}

	template <class T>
	inline void TokenList<T>::operator=(const TokenList<T>& list) {
		this->list = list.list;
		this->pos = list.pos;
	}

	template <class T>
	inline void TokenList<T>::operator=(TokenList<T>&& list) noexcept {
		this->list = std::move(list.list);
		this->pos = list.pos;
	}
}
