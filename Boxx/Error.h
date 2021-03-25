#pragma once

#include <stdexcept>
#include "Types.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	class String;

	///[Heading] Errors

	///[Title] Error
	/// Base class for all errors in the library.
	///[Block] Error
	class Error : public std::runtime_error {
	public:
		/// Creates an error with no message.
		///[Code] Error()
		Error() : runtime_error("") {}

		/// Creates an error with a message.
		///[Code] Error(const char* const msg)
		Error(const char* const msg) : runtime_error(msg) {}

		/// Returns the message of the error.
		String Message() const;

		/// Returns the name of the error class.
		///[para] All errors should override this method and return the name of the error class.
		virtual String Name() const;
	};

	///[Title] NotImplementedError
	/// Used if something has not been implemented yet.
	///[Block] NotImplementedError: Error
	class NotImplementedError : public Error {
	public:
		NotImplementedError() : Error() {}
		NotImplementedError(const char* const msg) : Error(msg) {}

		virtual String Name() const override;
	};

	///[Title] NotSupportedError
	/// Used if there is no implementation available.
	///[Block] NotSupportedError: Error
	class NotSupportedError : public Error {
	public:
		NotSupportedError() : Error() {}
		NotSupportedError(const char* const msg) : Error(msg) {}

		virtual String Name() const override;
	};

	///[Title] SystemNotSupportedError
	/// Thrown if the current operating system is not supported.
	///[Block] SystemNotSupportedError: NotSupportedError
	class SystemNotSupportedError : public NotSupportedError {
	public:
		SystemNotSupportedError() : NotSupportedError() {}
		SystemNotSupportedError(const char* const msg) : NotSupportedError(msg) {}

		virtual String Name() const override;
	};
}
