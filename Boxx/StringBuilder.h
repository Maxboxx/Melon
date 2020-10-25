#pragma once

#include "Types.h"
#include "String.h"
#include "List.h"

///N StringBuilder

namespace Boxx {
	///B StringBuilder
	/// A class used for constructing a string
	class StringBuilder {
	public:
		///H Constructors

		///T Empty String
		StringBuilder();

		///T Create from String
		///M
		StringBuilder(const char* str);
		StringBuilder(const String& str);
		///M

		StringBuilder(const StringBuilder& builder);
		StringBuilder(StringBuilder&& builder) noexcept;
		~StringBuilder();

		///H Methods

		///T To String
		/// Returns the constructed string
		String ToString() const;

		///T Copy
		/// Copies the <code>StringBuilder</code>
		StringBuilder Copy() const;

		///T Size
		/// Gets the size of the string
		UInt Size() const;

		///H Operators

		///T Append String
		/// Appends a string to the <code>StringBuilder</code>
		///M
		void operator+=(const char* str);
		void operator+=(const String& str);
		void operator+=(const StringBuilder& str);
		///M

		///T Assignment
		///M
		void operator=(const StringBuilder& builder);
		void operator=(StringBuilder&& builder) noexcept;
		///M

		///T Convert To String
		/// Returns the constructed string
		explicit operator String() const;

	private:
		List<char> string;
	};

	inline StringBuilder::StringBuilder() {
		
	}

	inline StringBuilder::StringBuilder(const char* str) {
		for (; *str != '\0'; str++) {
			string.Add(*str);
		}
	}

	inline StringBuilder::StringBuilder(const String& str) {
		for (const char c : str) {
			string.Add(c);
		}
	}

	inline StringBuilder::StringBuilder(const StringBuilder& builder) {
		string = builder.string;
	}

	inline StringBuilder::StringBuilder(StringBuilder&& builder) noexcept {
		string = std::move(builder.string);
	}

	inline StringBuilder::~StringBuilder() {
		
	}

	inline String StringBuilder::ToString() const {
		return String(string);
	}

	inline UInt StringBuilder::Size() const {
		return string.Size();
	}

	inline StringBuilder StringBuilder::Copy() const {
		StringBuilder sb;
		sb.string = string.Copy();
		return sb;
	}

	inline void StringBuilder::operator+=(const char* str) {
		for (; *str != '\0'; str++) {
			string.Add(*str);
		}
	}

	inline void StringBuilder::operator+=(const String& str) {
		for (const char c : str) {
			string.Add(c);
		}
	}

	inline void StringBuilder::operator+=(const StringBuilder& str) {
		for (const char c : str.string) {
			string.Add(c);
		}
	}

	inline void StringBuilder::operator=(const StringBuilder& builder) {
		string = builder.string;
	}

	inline void StringBuilder::operator=(StringBuilder&& builder) noexcept {
		string = std::move(builder.string);
	}

	inline StringBuilder::operator String() const {
		return String(string);
	}
}