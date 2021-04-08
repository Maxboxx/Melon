#include "Errors.h"

#include "Boxx/Map.h"
#include "Boxx/Console.h"

#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;
using namespace Melon;
using namespace Melon::Symbols;

Logger ErrorLog::logger = Logger("errors.log");
Logger ErrorLog::compileLogger = Logger("compile.log");
List<ErrorLog::Message> ErrorLog::messages;
List<UInt> ErrorLog::markers;
bool ErrorLog::stopOnError = false;

void ErrorLog::Log(const String& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Log;
	msg.message.message = message;
	msg.file = info;

	messages.Add(msg);
}

void ErrorLog::Info(const LogMessage& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Info;
	msg.message = message;
	msg.file = info;

	messages.Add(msg);
}

void ErrorLog::Success(const LogMessage& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Success;
	msg.message = message;
	msg.file = info;

	messages.Add(msg);
}

void ErrorLog::Warning(const LogMessage& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Warning;
	msg.message = message;
	msg.file = info;

	messages.Add(msg);
}

void ErrorLog::Error(const LogMessage& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Error;
	msg.message = message;
	msg.file = info;

	messages.Add(msg);

	if (stopOnError) {
		throw FatalLoggerError();
	}
}

void ErrorLog::Fatal(const LogMessage& message, const FileInfo& info) {
	Message msg;
	msg.level = LogLevel::Fatal;
	msg.message = message;
	msg.file = info;

	messages.Add(msg);
	throw FatalLoggerError();
}

void ErrorLog::AddMarker() {
	markers.Add(messages.Size());
}

void ErrorLog::RemoveMarker() {
	markers.RemoveAt(markers.Size() - 1);
}

void ErrorLog::RevertToMarker() {
	if (markers.Last() < messages.Size()) {
		messages.RemoveAt(markers.Last(), messages.Size() - markers.Last());
	}
}

void ErrorLog::Revert() {
	RevertToMarker();
	RemoveMarker();
}

void ErrorLog::LogErrors() {
	Map<String, List<Message>> sortedErrors;
	List<Message> noFileErrors;

	for (Message error : messages) {
		List<Message> errorList;

		if (error.file.line == 0) {
			noFileErrors.Add(error);
		}
		else {
			error.message.message = error.file.filename + ":" + error.file.line + " " + error.message.message;

			if (sortedErrors.Contains(error.file.filename, errorList)) {
				bool inserted = false;

				for (UInt i = 0; i < errorList.Size(); i++) {
					if (errorList[i].file.line == error.file.line) {
						if (errorList[i].message.message == error.message.message) {
							inserted = true;
							break;
						}
					}
					else if (errorList[i].file.line > error.file.line) {
						errorList.Insert(i, error);
						inserted = true;
						break;
					}
				}

				if (!inserted) {
					errorList.Add(error);
				}
			}
			else {
				errorList.Add(error);
				sortedErrors.Add(error.file.filename, errorList);
			}
		}
	}

	for (const Pair<String, List<Message>>& errorList : sortedErrors) {
		for (const Message& error : errorList.value) {
			switch (error.level) {
				case LogLevel::Log:     logger.Log(error.message.message); break;
				case LogLevel::Info:    logger.Info(error.message.message); break;
				case LogLevel::Success: logger.Success(error.message.message); break;
				case LogLevel::Warning: logger.Warning(error.message.message); break;
				case LogLevel::Error:   logger.Error(error.message.message); break;
				case LogLevel::Fatal: {
					logger.Write("fatal: " + error.message.message);
					Console::Print("fatal: " + error.message.message);
					break;
				}
			}
		}
	}

	for (const Message& error : noFileErrors) {
		switch (error.level) {
			case LogLevel::Log:     logger.Log(error.message.message); break;
			case LogLevel::Info:    logger.Info(error.message.message); break;
			case LogLevel::Success: logger.Success(error.message.message); break;
			case LogLevel::Warning: logger.Warning(error.message.message); break;
			case LogLevel::Error:   logger.Error(error.message.message); break;
			case LogLevel::Fatal: {
				logger.Write("fatal: " + error.message.message);
				Console::Print("fatal: " + error.message.message);
				break;
			}
		}
	}
}

bool ErrorLog::HasError() {
	for (const Message& error : messages) {
		if (error.level == LogLevel::Error) {
			return true;
		}
	}

	return false;
}

UInt ErrorLog::ErrorCount() {
	UInt count = 0;

	for (const Message& error : messages) {
		if (error.level == LogLevel::Error) {
			count++;
		}
	}

	return count;
}
