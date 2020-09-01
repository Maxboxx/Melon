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

///N String
namespace Boxx {
	///B String
	/// A class for handling strings of characters
	class String {
	public:
		///H Constructors

		///T Default
		/// Creates an empty string
		String();

		///T char array
		/// Creates a string from a char array
		///A const bool alloc: Only set this to <code>true</code> if you have allocated the array with <code>new</code> and want the class to delete it for you
		String(const char* const str, const bool alloc = false);

		///T Specific number of chars
		/// Creates a string from a specific number of chars from a char array
		///A const UInt bytes: The number of bytes to copy to the string
		///A const bool alloc: Only set this to <code>true</code> if you have allocated the array with <code>new</code> and want the class to delete it for you
		String(const char* const str, const UInt bytes, const bool alloc = false);

		///T std::string
		/// Converts an std::string to a String
		String(const std::string& str);

		///T char
		/// Converts a char to a string
		String(const char c);

		///T Array and List
		/// Converts an array or list of characters to a string
		///M
		explicit String(const Array<char>& arr);
		explicit String(const List<char>& list);
		///M

		///T Static array
		/// Converts a static array of characters to a string
		///M
		template <UInt S>
		explicit String(const StaticArray<char, S>& arr);
		///M
		
		String(const String& str);
		String(String&& str) noexcept;
		~String();

		///H Methods

		///T Size
		/// Gets the size of the string
		UInt Size() const;
		
		///T Substring
		/// Gets a substring from the string
		///A const UInt start: The starting index for the substring
		///A const UInt end: The ending index for the substring. Defaults to the last character of the string
		///M
		String Sub(const UInt start) const;
		String Sub(const UInt start, const UInt end) const;
		///M

		///T Split
		/// Splits a string into an array of substrings using a separator
		Array<String> Split(const String& sep) const;

		///T Trim
		/// Removes whitespace from the start and end of the string
		String Trim() const;

		///T Escape
		/// Escape characters in the string to make it printable
		String Escape() const;

		///T Lower
		/// Converts the string to lower case
		String Lower() const;

		///T Upper
		/// Converts the string to upper case
		String Upper() const;

		///T Replace
		/// Searches the string for a specific substring and replaces all occurrences of it with the specified value
		///A const String& search: The substring to search for
		///A const String& replace: The string to replace the substring with
		String Replace(const String& search, const String& replace) const;

		///H Operators

		void operator=(const String& str);
		void operator=(String&& str) noexcept;

		///T Concatenation
		/// Concatenates the string with a value
		///M
		String operator+(const String& s) const;
		String operator+(const char c) const;
		String operator+(const int i) const;
		String operator+(const unsigned int i) const;
		String operator+(const float f) const;
		String operator+(const double d) const;
		void operator+=(const String& s);
		void operator+=(const char c);
		void operator+=(const int i);
		void operator+=(const unsigned int i);
		void operator+=(const float f);
		void operator+=(const double d);
		///M

		///T Equality operators
		/// Compares two strings to check if they are equal or not
		///M
		bool operator==(const char* const s) const;
		bool operator==(const String& s) const;
		bool operator!=(const char* const s) const;
		bool operator!=(const String& s) const;
		///M

		///T Comparison operators
		/// Compares two strings in character order
		///M
		bool operator<(const char* const s) const;
		bool operator<(const String& s) const;
		bool operator>(const char* const s) const;
		bool operator>(const String& s) const;
		bool operator<=(const char* const s) const;
		bool operator<=(const String& s) const;
		bool operator>=(const char* const s) const;
		bool operator>=(const String& s) const;
		///M

		///T Index operator
		/// Gets the character at the specified position of the string
		char operator[](const int i) const;

		///H Conversion operators

		///T Char array conversion
		/// Conversion to a character pointer
		operator const char*() const;

		///H Conversion functions
		
		///T Convert to string
		/// Converts a value to a string
		///M
		static String ToString(const bool b);
		static String ToString(const Int i);
		static String ToString(const UInt i);
		static String ToString(const Long l);
		static String ToString(const ULong l);
		static String ToString(const float f);
		static String ToString(const double d);
		///M
		
		///T Convert to int
		/// Converts the string from a specific base to an int
		Int ToInt(const int base = 10) const;

		///T Convert to unsigned int
		/// Converts the string from a specific base to an unsigned int
		UInt ToUInt(const int base = 10) const;

		///T Convert to long
		/// Converts the string from a specific base to a long
		Long ToLong(const int base = 10) const;

		///T Convert to unsigned long
		/// Converts the string from a specific base to an unsigned long
		ULong ToULong(const int base = 10) const;

		///T Convert to float
		/// Converts the string to a float
		float ToFloat() const;

		///T Convert to double
		/// Converts the string to a double
		double ToDouble() const;

		///H Iterator
		///T Char iterator
		/// Iterates over each <code>char</code> in the string
		///S for (char c : string)
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
