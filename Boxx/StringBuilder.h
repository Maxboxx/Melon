#pragma once

#include "Types.h"
#include "String.h"
#include "List.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Title] StringBuilder
	/// A class used for constructing a string.
	///[Block] StringBuilder
	class StringBuilder {
	public:
		///[Heading] Constructors

		/// Creates an empty string.
		StringBuilder();

		/// Creates a string from a char array.
		StringBuilder(const char* str);

		/// Create from a string.
		StringBuilder(const String& str);

		StringBuilder(const StringBuilder& builder);
		StringBuilder(StringBuilder&& builder) noexcept;
		~StringBuilder();

		///[Heading] Methods

		/// Returns the constructed string.
		String ToString() const;

		/// Copies the {StringBuilder}.
		StringBuilder Copy() const;

		/// Gets the size of the string.
		UInt Size() const;

		///[Heading] Operators

		/// Appends a string to the {StringBuilder}.
		void operator+=(const char* str);

		/// Appends a string to the {StringBuilder}.
		void operator+=(const String& str);

		/// Appends a string to the {StringBuilder}.
		void operator+=(const StringBuilder& str);

		void operator=(const StringBuilder& builder);
		void operator=(StringBuilder&& builder) noexcept;

		/// Returns the constructed string.
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