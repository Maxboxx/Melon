#pragma once

#include "Types.h"
#include "String.h"
#include "Error.h"
#include "File.h"
#include "Console.h"
#include "Pointer.h"
#include "List.h"
#include "Pair.h"
#include "Optional.h"
#include "Tuple.h"

///N Logger

namespace Boxx {
	///B Logger
	/// Used to log information to a file and/or console
	class Logger {
	public:

		///H Constructors

		///T Console constructor
		/// A logger created this way can only log to the console
		Logger();

		///T Create log file
		/// Creates a logger and opens a file for it
		///E FileOpenError: Thrown if the file could not be opened
		explicit Logger(const String& filename);

		Logger(const Logger& logger);
		Logger(Logger&& logger) noexcept;
		~Logger();

		///H Logging Methods

		///T Write
		/// Writes to the log file
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Write(const String& str);

		///T Log
		/// Writes a log to the log file
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Log(const String& str);

		///T Info
		/// Writes info to the log file and the console
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Info(const String& str);

		///T Success
		/// Writes a success message to the log file and the console
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Success(const String& str);

		///T Warning
		/// Writes a warning to the log file and the console
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Warning(const String& str);

		///T Error
		/// Writes an error to the log file and the console
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Error(const String& str);

		///T Fatal
		/// Writes a fatal error to the log file and the console
		///E FatalLoggerError: Always thrown
		///E LoggerFileError: Thrown if the logger has created a log file and the file is not open
		void Fatal(const String& str);

		///H Methods

		///T Close
		/// Close the log file
		///E LoggerFileError: Thrown if the log file is not open
		void Close();

		///T Is Open
		/// Checks if the log file is open
		bool IsOpen() const;

		///H Messages

		////B LogLevel
		//// The log level of a message
		enum class LogLevel : UByte {
			////T Values
			////M
			Write,
			Log,
			Info,
			Success,
			Warning,
			Error,
			Fatal
			////M
		};

		///T Iterator
		/// Iterates over all logged messages
		///S for (Tuple<Logger::LogLevel, String> message : logger)
		const Tuple<LogLevel, String>* begin() const;
		const Tuple<LogLevel, String>* end() const;

		void operator=(const Logger& logger);
		void operator=(Logger&& logger) noexcept;

	private:
		Optional<FileWriter> file;
		List<Tuple<LogLevel, String>> loggedMessages;
	};

	///B LoggerError
	/// Base class for all logger erros
	class LoggerError : public Error {
	public:
		LoggerError() : Error() {}
		LoggerError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "LoggerError";
		}
	};

	///B FatalLoggerError
	/// Used for fatal logs
	class FatalLoggerError : public LoggerError {
	public:
		FatalLoggerError() : LoggerError() {}
		FatalLoggerError(const char* const msg) : LoggerError(msg) {}

		virtual String Name() const override {
			return "FatalLoggerError";
		}
	};

	///B LoggerFileError
	/// Used for log file related errors
	class LoggerFileError : public LoggerError {
	public:
		LoggerFileError() : LoggerError() {}
		LoggerFileError(const char* const msg) : LoggerError(msg) {}

		virtual String Name() const override {
			return "LoggerFileError";
		}
	};

	inline Logger::Logger() {
		
	}

	inline Logger::Logger(const String& filename) {
		try {
			file = FileWriter(filename);
		}
		catch (FileOpenError e) {
			throw e;
		}
	}

	inline Logger::Logger(const Logger& logger) {
		file = logger.file;
		loggedMessages = logger.loggedMessages;
	}

	inline Logger::Logger(Logger&& logger) noexcept {
		file = std::move(logger.file);
		loggedMessages = std::move(logger.loggedMessages);
	}

	inline Logger::~Logger() {
	
	}

	inline void Logger::Write(const String& str) {
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Write, str + "\n"));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write(str + "\n");
	}

	inline void Logger::Log(const String& str) {
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Log, "log: " + str + "\n"));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write("log: " + str + "\n");
	}

	inline void Logger::Info(const String& str) {
		const String s = "info: " + str + "\n";
		Console::Write(s);
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Info, s));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write(s);
	}

	inline void Logger::Success(const String& str) {
		const String s = "success: " + str + "\n";
		Console::Write(s);
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Success, s));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write(s);
	}

	inline void Logger::Warning(const String& str) {
		const String s = "warning: " + str + "\n";
		Console::Write(s);
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Warning, s));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write(s);
	}

	inline void Logger::Error(const String& str) {
		const String s = "error: " + str + "\n";
		Console::Write(s);
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Error, s));
		if (!file) return;
		if (!file->IsOpen()) throw LoggerFileError("File is not open");
		file->Write(s);
	}

	inline void Logger::Fatal(const String& str) {
		const String s = "fatal: " + str + "\n";
		Console::Write(s);
		loggedMessages.Add(Tuple<LogLevel, String>(LogLevel::Fatal, s));
		if (file) {
			if (!file->IsOpen()) throw LoggerFileError("File is not open");
			file->Write(s);
		}
		throw FatalLoggerError(str);
	}

	inline void Logger::Close() {
		if (!file || !file->IsOpen()) throw LoggerFileError("File is not open");
		file->Close();
	}

	inline bool Logger::IsOpen() const {
		if (!file) return false;
		return file->IsOpen();
	}

	inline const Tuple<Logger::LogLevel, String>* Logger::begin() const {
		return loggedMessages.begin();
	}

	inline const Tuple<Logger::LogLevel, String>* Logger::end() const {
		return loggedMessages.end();
	}

	inline void Logger::operator=(const Logger& logger) {
		file = logger.file;
		loggedMessages = logger.loggedMessages;
	}

	inline void Logger::operator=(Logger&& logger) noexcept {
		file = std::move(logger.file);
		loggedMessages = std::move(logger.loggedMessages);
	}
}
