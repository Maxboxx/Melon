#pragma once

#include "Boxx/Error.h"
#include "Boxx/String.h"
#include "Boxx/Logger.h"
#include "Boxx/Tuple.h"
#include "Boxx/Pair.h"
#include "Boxx/List.h"
#include "Boxx/Set.h"

#include "Melon/Symbols/NameList.h"

#include "LogMessage.h"
#include "FileInfo.h"

///N Melon
namespace Melon {
	namespace Symbols {
		class FunctionSymbol;
	}

	/// Static class for logging errors.
	class ErrorLog {
	public:
		/// Creates a log message.
		static void Log(const Boxx::String& message, const FileInfo& info);

		/// Creates an info message.
		static void Info(const LogMessage& message, const FileInfo& info);

		/// Creates a success message.
		static void Success(const LogMessage& message, const FileInfo& info);

		/// Creates a warning message.
		static void Warning(const LogMessage& message, const FileInfo& info);

		/// Creates an error message.
		static void Error(const LogMessage& message, const FileInfo& info);

		/// Creates a fatal message.
		static void Fatal(const LogMessage& message, const FileInfo& info);

		/// Adds a marker to the error log.
		static void AddMarker();

		/// Removes the last marker from the error log.
		static void RemoveMarker();

		/// Revert back to the last marker.
		static void RevertToMarker();

		/// Revert back to the last marker and removes the marker.
		static void Revert();

		/// Log the errors to a log file.
		static void LogErrors();

		/// Check if the error log has errors.
		static bool HasError();

		/// Get the error count.
		static Boxx::UInt ErrorCount();

		/// {true} if the compiler should stop on the first error.
		///p Default value is {false}.
		static bool stopOnError;

		/// Compile logger.
		static Boxx::Logger compileLogger;

		/// Logger.
		static Boxx::Logger logger;

	private:
		enum class LogLevel {
			Log,
			Info,
			Success,
			Warning,
			Error,
			Fatal
		};

		struct Message {
			LogLevel level;
			LogMessage message;
			FileInfo file;

			Message() {}
			Message(const Message& msg) {
				level = msg.level;
				message = msg.message;
				file = msg.file;
			}

			~Message() {}
		};

		static Boxx::List<Message> messages;
		static Boxx::List<Boxx::UInt> markers;
	};

	/// Used for compile errors.
	class CompileError : public Boxx::Error {
	public:
		CompileError() : Error() {}
		CompileError(const char* const msg) : Error(msg) {}

		virtual Boxx::String Name() {
			return "CompileError";
		}
	};
}
