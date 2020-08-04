#pragma once

#include <stdexcept>
#include "Types.h"

///N Error
namespace Boxx {
	class String;

	///B Error
	/// Base class for all errors in the library
	class Error : public std::runtime_error {
	public:
		Error() : runtime_error("") {}
		Error(const char* const msg) : runtime_error(msg) {}

		///T Get message
		/// Returns the error message of the error
		String Message() const;
	};

	///B SystemNotSupportedError
	/// Thrown if the current operating system is not supported
	class SystemNotSupportedError : public Error {
	public:
		SystemNotSupportedError() : Error() {}
		SystemNotSupportedError(const char* const msg) : Error(msg) {}
	};
}
