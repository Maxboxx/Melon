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

		///T Name
		/// Returns the name of the error class
		/// All errors should override this method
		virtual String Name() const;
	};

	///B NotImplementedError
	/// Thrown if something has not been implemented yet
	class NotImplementedError : public Error {
	public:
		NotImplementedError() : Error() {}
		NotImplementedError(const char* const msg) : Error(msg) {}

		virtual String Name() const override;
	};

	///B NotSupportedError
	/// Thrown if there is no implementation available
	class NotSupportedError : public Error {
	public:
		NotSupportedError() : Error() {}
		NotSupportedError(const char* const msg) : Error(msg) {}

		virtual String Name() const override;
	};

	///B SystemNotSupportedError
	/// Thrown if the current operating system is not supported
	class SystemNotSupportedError : public NotSupportedError {
	public:
		SystemNotSupportedError() : NotSupportedError() {}
		SystemNotSupportedError(const char* const msg) : NotSupportedError(msg) {}

		virtual String Name() const override;
	};
}
