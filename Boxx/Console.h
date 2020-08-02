#pragma once

#include <iostream>

#include "Types.h"
#include "String.h"

#include <string>

///N Console

namespace Boxx {
	///B Console
	/// Static class used for reading/writing to the console
	class Console {
	public:
		
		///T Write
		/// Writes something to the console
		///M
		template <class T>
		static void Write(const T& info);
		///M

		///T Empty print
		/// Prints a line break to the console
		static void Print();

		///T Print
		/// Writes something to the console followed by a line break
		///M
		template <class T>
		static void Print(const T& info);
		///M

		///T Read
		/// Reads input from the console
		static String Read();
	};

	template <class T>
	inline void Console::Write(const T& info) {
		std::cout << info;
	}

	template <>
	inline void Console::Write(const String& info) {
		std::cout << (const char*)info;
	}

	template <>
	inline void Console::Write(const bool& info) {
		std::cout << (info ? "true" : "false");
	}

	inline void Console::Print() {
		std::cout << std::endl;
	}

	template <class T>
	inline void Console::Print(const T& info) {
		std::cout << info << std::endl;
	}

	template <>
	inline void Console::Print(const String& info) {
		std::cout << (const char*)info << std::endl;
	}

	template <>
	inline void Console::Print(const bool& info) {
		std::cout << (info ? "true" : "false") << std::endl;
	}

	inline String Console::Read() {
		std::string line;
		std::getline(std::cin, line);
		return line;
	}
}
