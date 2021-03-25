#pragma once

#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "Error.h"
#include "Types.h"
#include "List.h"
#include "Array.h"
#include "StaticArray.h"
#include "Optional.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Title] String
	/// A string of characters.
	///[Block] String
	class String final {
	public:
		///[Heading] Constructors

		/// Creates an empty string.
		String();

		/// Creates a string from a {char} array.
		///[Arg] alloc: Only set this to {true} if you have allocated {str} with {new} and want the string to delete it for you.
		String(const char* const str, const bool alloc = false);

		/// Creates a string from a specific number of chars from a {char} array.
		///[Arg] bytes: The number of bytes to copy to the string.
		///[Arg] alloc: Only set this to {true} if you have allocated {str} with {new} and want the string to delete it for you.
		String(const char* const str, const UInt bytes, const bool alloc = false);

		/// Converts an {std::string} to a {String}.
		String(const std::string& str);

		/// Converts a {char} to a string.
		String(const char c);

		/// Converts an array of characters to a string.
		explicit String(const Array<char>& arr);

		/// Converts a list of characters to a string.
		explicit String(const List<char>& list);

		/// Converts a static array of characters to a string.
		///M
		template <UInt S>
		explicit String(const StaticArray<char, S>& arr);
		///M
		
		String(const String& str);
		String(String&& str) noexcept;
		~String();

		///[Heading] Methods

		/// Gets the size of the string.
		UInt Size() const;
		
		/// Gets a substring from the string.
		///[Arg] start: The starting index for the substring.
		String Sub(const UInt start) const;

		/// Gets a substring from the string.
		///[Arg] start: The starting index for the substring.
		///[Arg] end: The ending index for the substring.
		String Sub(const UInt start, const UInt end) const;

		/// Splits a string into an array of substrings using a separator.
		Array<String> Split(const String& sep) const;

		/// Removes whitespace from the start and end of the string.
		String Trim() const;

		/// Escape characters in the string to make it printable.
		String Escape() const;

		/// Converts the string to lower case.
		String Lower() const;

		/// Converts the string to upper case.
		String Upper() const;

		/// Searches the string for a specific substring and replaces all occurrences of it with the specified value.
		///[Arg] search: The substring to search for.
		///[Arg] replace: The string to replace the substring with.
		String Replace(const String& search, const String& replace) const;

		/// Repeats a string {rep} times.
		String Repeat(const UInt rep) const;

		/// Finds the position of the specified substring.
		///[Arg] start: The position to start the search at.
		///[Returns] Optional<UInt>: The position of the string where {search} was found.
		///[para] Does not contain a value if {search} was not found.
		Optional<UInt> Find(const String& search, const UInt start = 0) const;

		///[Heading] Operators

		void operator=(const String& str);
		void operator=(String&& str) noexcept;

		/// Concatenates the string with another string.
		String operator+(const String& s) const;

		/// Concatenates the string with a character.
		String operator+(const char c) const;

		/// Concatenates the string with an integer.
		String operator+(const int i) const;

		/// Concatenates the string with an unsigned integer.
		String operator+(const unsigned int i) const;

		/// Concatenates the string with a float.
		String operator+(const float f) const;

		/// Concatenates the string with a double.
		String operator+(const double d) const;

		void operator+=(const String& s);
		void operator+=(const char c);
		void operator+=(const int i);
		void operator+=(const unsigned int i);
		void operator+=(const float f);
		void operator+=(const double d);

		/// Compares two strings to check if they are equal.
		bool operator==(const char* const s) const;

		/// Compares two strings to check if they are equal.
		bool operator==(const String& s) const;

		/// Compares two strings to check if they are not equal.
		bool operator!=(const char* const s) const;

		/// Compares two strings to check if they are not equal.
		bool operator!=(const String& s) const;

		/// Compares two strings in character order.
		bool operator<(const char* const s) const;

		/// Compares two strings in character order.
		bool operator<(const String& s) const;

		/// Compares two strings in character order.
		bool operator>(const char* const s) const;

		/// Compares two strings in character order.
		bool operator>(const String& s) const;

		/// Compares two strings in character order.
		bool operator<=(const char* const s) const;

		/// Compares two strings in character order.
		bool operator<=(const String& s) const;

		/// Compares two strings in character order.
		bool operator>=(const char* const s) const;

		/// Compares two strings in character order.
		bool operator>=(const String& s) const;

		/// Gets the character at the specified position of the string.
		char operator[](const int i) const;

		/// Converts the string to a character pointer.
		operator const char*() const;

		///[Heading] Conversion functions
		
		/// Converts a {bool} to a string.
		static String ToString(const bool b);

		/// Converts an {Int} to a string.
		static String ToString(const Int i);

		/// Converts a {UInt} to a string.
		static String ToString(const UInt i);

		/// Converts a {Long} to a string.
		static String ToString(const Long l);

		/// Converts a {ULong} to a string.
		static String ToString(const ULong l);

		/// Converts a {float} to a string.
		static String ToString(const float f);

		/// Converts a {double} to a string.
		static String ToString(const double d);
		
		/// Converts the string from a specific base to an int.
		Int ToInt(const int base = 10) const;

		/// Converts the string from a specific base to an unsigned int.
		UInt ToUInt(const int base = 10) const;

		/// Converts the string from a specific base to a long.
		Long ToLong(const int base = 10) const;

		/// Converts the string from a specific base to an unsigned long.
		ULong ToULong(const int base = 10) const;

		/// Converts the string to a float.
		float ToFloat() const;

		/// Converts the string to a double.
		double ToDouble() const;

		///[Heading] Iterators

		/// Iterates over each {char} in the string.
		///[Code] for (char c : string)
		const char* begin() const;
		const char* end() const;

	private:
		char* str;
		UInt len;
		bool alloc = true;
	};

	inline String Error::Message() const {
		return String(what());
	}

	inline String Error::Name() const {
		return "Error";
	}

	inline String NotImplementedError::Name() const {
		return "NotImplementedError";
	}

	inline String NotSupportedError::Name() const {
		return "NotSupportedError";
	}

	inline String SystemNotSupportedError::Name() const {
		return "SystemNotSupportedError";
	}

	inline String OptionalError::Name() const  {
		return "OptionalError";
	}

	inline String ArrayError::Name() const {
		return "ArrayError";
	}

	inline String ArraySizeError::Name() const {
		return "ArraySizeError";
	}

	inline String::String() {
		str = new char[1] {'\0'};
		len = 0;
	}

	inline String::String(const char* const str, const bool alloc) {
		int i = 0;
		this->alloc = !alloc;

		while (str[i++] != '\0');
		len = i - 1;

		if (!alloc) {
			this->str = new char[i];
			std::memcpy(this->str, str, sizeof(char) * i);
		}
		else {
			this->str = (char*)str;
		}
	}

	inline String::String(const char* const str, const UInt bytes, const bool alloc) {
		len = bytes;
		this->alloc = !alloc;

		if (!alloc) {
			this->str = new char[(ULong)len + 1];
			std::memcpy(this->str, str, sizeof(char) * len);
			this->str[len] = '\0';
		}
		else {
			this->str = (char*)str;
		}
	}

	inline String::String(const std::string& str) {
		len = (UInt)str.length();
		this->str = new char[(ULong)len + 1];
		this->str[len] = '\0';
		alloc = true;
		std::memcpy(this->str, str.c_str(), sizeof(char) * ((ULong)len + 1));
	}

	inline String::String(const char c) {
		len = 1;
		str = new char[2]{c, '\0'};
	}

	inline String::String(const Array<char>& arr) {
		len = arr.Size();
		this->str = new char[(ULong)len + 1];
		str[len] = '\0';
		alloc = true;
		std::memcpy(this->str, (const char*)arr, sizeof(char) * len);
	}

	inline String::String(const List<char>& list) {
		len = list.Size();
		this->str = new char[(ULong)len + 1];
		str[len] = '\0';
		alloc = true;
		std::memcpy(this->str, list.list->list, sizeof(char) * len);
	}

	template <UInt S>
	inline String::String(const StaticArray<char, S>& arr) {
		len = S;
		this->str = new char[(ULong)len + 1];
		str[len] = '\0';
		alloc = true;
		std::memcpy(this->str, (const char*)arr, sizeof(char) * len);
	}

	inline String::String(const String& str) {
		len = str.len;
		this->str = new char[(ULong)len + 1];
		alloc = true;
		std::memcpy(this->str, str.str, sizeof(char) * ((ULong)len + 1));
	}

	inline String::String(String&& str) noexcept {
		this->str = str.str;
		len = str.len;
		alloc = str.alloc;
		str.str = nullptr;
		str.alloc = false;
	}

	inline String::~String() {
		if (alloc)
			delete[] str;
	}

	inline UInt String::Size() const {
		return len;
	}

	inline String String::Sub(const UInt start) const {
		char* const cstr = new char[(ULong)len - start + 1];
		std::memcpy(cstr, str + start, sizeof(char) * (len - start));
		cstr[len - start] = '\0';
		return String(cstr, len - start, true);
	}

	inline String String::Sub(const UInt start, const UInt end) const {
		char* const cstr = new char[(ULong)end - start + 2];
		std::memcpy(cstr, str + start, sizeof(char) * ((ULong)end - start + 1));
		cstr[end - start + 1] = '\0';
		return String(cstr, end - start + 1, true);
	}

	inline Array<String> String::Split(const String& sep) const {
		List<String> strings;
		int start = 0;

		for (UInt i = 0; i < Size(); i++) {
			const UInt end = i + sep.Size();
			bool found = true;

			for (UInt u = i, k = 0; u < end; u++) {
				if (u >= Size() || str[u] != sep.str[k++]) {
					found = false;
					break;
				}
			}

			if (found) {
				strings.Add(Sub(start, i - 1));
				start = end;
			}
		}

		strings.Add(Sub(start));

		return strings.ToArray();
	}

	inline String String::Trim() const {
		static const char whitespace[] = {' ', '\t', '\n', '\r', '\v', '\0'};

		if (Size() == 0) return String();

		UInt start;
		Long end;

		for (start = 0; start < Size(); start++) {
			bool white = false;

			for (UInt i = 0; i < 6; i++) {
				if (str[start] == whitespace[i]) {
					white = true;
					break;
				}
			}

			if (!white) {
				break;
			}
		}

		for (end = (Long)Size() - 1; end >= 0; end--) {
			bool white = false;

			for (UInt i = 0; i < 6; i++) {
				if (str[end] == whitespace[i]) {
					white = true;
					break;
				}
			}

			if (!white) {
				break;
			}
		}

		return Sub(start, (UInt)end);
	}

	inline String String::Escape() const {
		List<char> chars{Size() + 1};

		for (const char c : *this) {
			switch (c) {
			case '\n':
				chars.Add('\\');
				chars.Add('n');
				break;
			case '\\':
				chars.Add('\\');
				chars.Add('\\');
				break;
			case '\t':
				chars.Add('\\');
				chars.Add('t');
				break;
			case '\r':
				chars.Add('\\');
				chars.Add('r');
				break;
			case '\"':
				chars.Add('\\');
				chars.Add('"');
				break;
			default:
				chars.Add(c);
				break;
			}
		}

		return String(chars.list->list, chars.Size());
	}

	inline String String::Lower() const {
		List<char> chars{Size()};

		for (const char c : *this) {
			if (c >= 'A' && c <= 'Z') {
				chars.Add(c - 'A' + 'a');
			}
			else {
				chars.Add(c);
			}
		}

		return String(chars.list->list, chars.Size());
	}

	inline String String::Upper() const {
		List<char> chars{Size()};

		for (const char c : *this) {
			if (c >= 'a' && c <= 'z') {
				chars.Add(c - 'a' + 'A');
			}
			else {
				chars.Add(c);
			}
		}

		return String(chars.list->list, chars.Size());
	}

	inline String String::Replace(const String& search, const String& replace) const {
		List<char> chars{Size()};

		for (UInt i = 0; i < Size(); i++) {
			if (str[i] == search.str[0] && Size() - i >= search.Size()) {
				bool found = true;

				for (UInt u = 1; u < search.Size(); u++) {
					if (str[i + u] != search.str[u]) {
						found = false;
						break;
					}
				}

				if (found) {
					for (const char c : replace) {
						chars.Add(c);
					}

					i += search.Size() - 1;
				}
				else {
					chars.Add(str[i]);
				}
			}
			else {
				chars.Add(str[i]);
			}
		}

		return String(chars.list->list, chars.Size());
	}

	inline String String::Repeat(const UInt rep) const {
		const UInt len = Size();
		char* str = new char[len * rep + 1];

		for (UInt i = 0; i < rep; i++) {
			std::memcpy(str + i * len, this->str, sizeof(char) * len);
		}

		str[len * rep] = '\0';

		return String(str, len * rep, true);
	}

	inline Optional<UInt> String::Find(const String& search, const UInt start) const {
		for (UInt i = start; i < Size(); i++) {
			if (str[i] == search.str[0] && Size() - i >= search.Size()) {
				bool found = true;

				for (UInt u = 1; u < search.Size(); u++) {
					if (str[i + u] != search.str[u]) {
						found = false;
						break;
					}
				}

				if (found) {
					return i;
				}
			}
			else {
				return nullptr;
			}
		}

		return nullptr;
	}

	inline void String::operator=(const String& str) {
		if (alloc)
			delete[] this->str;

		len = str.len;
		this->str = new char[(ULong)len + 1];
		alloc = true;
		std::memcpy(this->str, str.str, sizeof(char) * ((ULong)len + 1));
	}

	inline void String::operator=(String&& str) noexcept {
		this->str = str.str;
		len = str.len;
		alloc = str.alloc;
		str.str = nullptr;
		str.alloc = false;
	}

	inline String String::operator+(const String& s) const {
		char* const cstr = new char[(ULong)len + s.len + 1];
		std::memcpy(cstr, str, sizeof(char) * len);
		std::memcpy(cstr + len, s.str, sizeof(char) * ((ULong)s.len + 1));
		return String(cstr, len + s.len, true);
	}

	inline String String::operator+(const char c) const {
		char* const cstr = new char[(ULong)len + 2];
		std::memcpy(cstr, str, sizeof(char) * len);
		cstr[len] = c;
		cstr[len + 1] = '\0';
		return String(cstr, len + 1, true);
	}

	inline String String::operator+(const int i) const {
		return *this + String::ToString(i);
	}

	inline String String::operator+(const unsigned int i) const {
		return *this + String::ToString(i);
	}

	inline String String::operator+(const float f) const {
		return *this + String::ToString(f);
	}

	inline String String::operator+(const double d) const {
		return *this + String::ToString(d);
	}

	inline void String::operator+=(const String& s) {
		*this = *this + s;
	}

	inline void String::operator+=(const char c) {
		*this = *this + c;
	}

	inline void String::operator+=(const int i) {
		*this = *this + i;
	}

	inline void String::operator+=(const unsigned int i) {
		*this = *this + i;
	}

	inline void String::operator+=(const float f) {
		*this = *this + f;
	}

	inline void String::operator+=(const double d) {
		*this = *this + d;
	}

	inline bool operator==(const char* const s, const String& str) {
		return str == s;
	}

	inline bool String::operator==(const char* const s) const {
		for (UInt i = 0; i < len; i++)
			if (s[i] == '\0' || str[i] != s[i])
				return false;

		return s[len] == '\0';
	}

	inline bool String::operator==(const String& s) const {
		if (s.len != len)
			return false;

		for (UInt i = 0; i < len; i++)
			if (str[i] != s[i])
				return false;

		return true;
	}

	inline bool operator!=(const char* const s, const String& str) {
		return str != s;
	}

	inline bool String::operator!=(const char* const s) const {
		return !operator==(s);
	}

	inline bool String::operator!=(const String& s) const {
		return !operator==(s);
	}

	inline bool operator<(const char* const s, const String& str) {
		return str > s;
	}

	inline bool String::operator<(const char* const s) const {
		for (UInt i = 0; i < len; i++) {
			if (str[i] < s[i])
				return true;
			else if (str[i] > s[i])
				return false;
		}

		return s[len] != '\0';
	}

	inline bool String::operator<(const String& s) const {
		const UInt size = Size() < s.Size() ? Size() : s.Size();

		for (UInt i = 0; i < size; i++) {
			if (str[i] < s.str[i])
				return true;
			else if (str[i] > s.str[i]) {
				return false;
			}
		}

		return Size() < s.Size();
	}

	inline bool operator>(const char* const s, const String& str) {
		return str < s;
	}

	inline bool String::operator>(const char* const s) const {
		return s < *this;
	}

	inline bool String::operator>(const String& s) const {
		return s < *this;
	}

	inline bool operator<=(const char* const s, const String& str) {
		return str >= s;
	}

	inline bool String::operator<=(const char* const s) const {
		for (UInt i = 0; i < len; i++) {
			if (str[i] < s[i])
				return true;
			else if (str[i] > s[i])
				return false;
		}

		return s[len] == '\0';
	}

	inline bool String::operator<=(const String& s) const {
		const UInt size = Size() < s.Size() ? Size() : s.Size();

		for (UInt i = 0; i < size; i++) {
			if (str[i] < s.str[i])
				return true;
			else if (str[i] > s.str[i]) {
				return false;
			}
		}

		return true;
	}

	inline bool operator>=(const char* const s, const String& str) {
		return str <= s;
	}

	inline bool String::operator>=(const char* const s) const {
		return s <= *this;
	}

	inline bool String::operator>=(const String& s) const {
		return s <= *this;
	}

	inline String::operator const char*() const {
		return str;
	}

	inline char String::operator[](const int i) const {
		return str[i];
	}

	inline String String::ToString(const bool b) {
		return b ? "true" : "false";
	}

	inline String String::ToString(const Int i) {
		return std::to_string(i).c_str();
	}

	inline String String::ToString(const UInt i){
		return std::to_string(i).c_str();
	}

	inline String String::ToString(const Long l) {
		return std::to_string(l).c_str();
	}

	inline String String::ToString(const ULong l){
		return std::to_string(l).c_str();
	}

	inline String String::ToString(const float f) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(20) << f;
		const String str = oss.str().c_str();

		for (UInt i = str.Size(); i > 0; i--) {
			if (str[i] == '.')
				return str.Sub(0, i + 1);
			else if (str[i] != '0' && str[i] != '\0')
				return str.Sub(0, i);
		}

		return str;
	}

	inline String String::ToString(const double d) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(20) << d;
		const String str = oss.str().c_str();

		for (UInt i = str.Size(); i > 0; i--) {
			if (str[i] == '.')
				return str.Sub(0, i + 1);
			else if (str[i] != '0' && str[i] != '\0')
				return str.Sub(0, i);
		}

		return str;
	}

	inline Int String::ToInt(const int base) const {
		return strtol(str, nullptr, base);
	}

	inline UInt String::ToUInt(const int base) const {
		return strtoul(str, nullptr, base);
	}

	inline Long String::ToLong(const int base) const {
		return strtoll(str, nullptr, base);
	}

	inline ULong String::ToULong(const int base) const {
		return strtoull(str, nullptr, base);
	}

	inline float String::ToFloat() const {
		return strtof(str, nullptr);
	}

	inline double String::ToDouble() const {
		return strtod(str, nullptr);
	}

	inline const char* String::begin() const {
		return str;
	}

	inline const char* String::end() const {
		return &str[len];
	}

	inline String operator+(const char* const str, const String& s) {
		return String(str) + s;
	}

	inline String operator+(const char c, const String& s) {
		return String(c) + s;
	}

	inline String operator+(const int i, const String& s) {
		return String::ToString(i) + s;
	}

	inline String operator+(const float f, const String& s) {
		return String::ToString(f) + s;
	}
}
