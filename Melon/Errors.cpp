#include "Errors.h"

#include "Boxx/Map.h"

using namespace Boxx;
using namespace Melon;

Logger ErrorLog::logger = Logger("errors.log");
Logger ErrorLog::compileLogger = Logger("compile.log");
List<Tuple<ErrorLog::LogLevel, CompileError>> ErrorLog::errors;
List<UInt> ErrorLog::markers;
bool ErrorLog::stopOnError = false;

void ErrorLog::Log(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Log, error));
}

void ErrorLog::Info(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Info, error));
}

void ErrorLog::Warning(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Warning, error));
}

void ErrorLog::Error(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Error, error));

	if (stopOnError) {
		throw error;
	}
}

void ErrorLog::Fatal(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Fatal, error));
	logger.Fatal(error.Message());
}

void ErrorLog::AddMark() {
	markers.Add(errors.Size());
}

void ErrorLog::RemoveMark() {
	markers.RemoveAt(markers.Size() - 1);
}

void ErrorLog::RevertToMark() {
	if (markers.Last() < errors.Size()) {
		errors.RemoveAt(markers.Last(), errors.Size() - markers.Last());
	}
}

void ErrorLog::LogErrors() {
	Map<String, List<Tuple<LogLevel, CompileError>>> sortedErrors;

	for (const Tuple<LogLevel, CompileError>& error : errors) {
		List<Tuple<LogLevel, CompileError>> errorList;

		if (sortedErrors.Contains(error.value2.file.filename, errorList)) {
			bool inserted = false;

			for (UInt i = 0; i < errorList.Size(); i++) {
				if (errorList[i].value2.file.line == error.value2.file.line) {
					if (errorList[i].value2.Message() == error.value2.Message()) {
						inserted = true;
						break;
					}
				}
				else if (errorList[i].value2.file.line > error.value2.file.line) {
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
			sortedErrors.Add(error.value2.file.filename, errorList);
		}
	}

	for (const Pair<String, List<Tuple<LogLevel, CompileError>>>& errorList : sortedErrors) {
		for (const Tuple<LogLevel, CompileError>& error : errorList.value) {
			//if (errorList.key.Size() > 1) {
				switch (error.value1) {
					case LogLevel::Log:     logger.Log(error.value2.Message()); break;
					case LogLevel::Info:    logger.Info(error.value2.Message()); break;
					case LogLevel::Warning: logger.Warning(error.value2.Message()); break;
					case LogLevel::Error:   logger.Error(error.value2.Message()); break;
					case LogLevel::Fatal:   logger.Fatal(error.value2.Message()); break;
				}
			//}
		}
	}
}

bool ErrorLog::HasError() {
	for (const Tuple<LogLevel, CompileError>& error : errors) {
		if (error.value1 == LogLevel::Error) {
			return true;
		}
	}

	return false;
}

const char* const CompileError::SelfInit          = "'self' used before all member variables are initialized";
const char* const CompileError::InvalidRef        = "invalid reference value";
const char* const CompileError::FuncNotFound      = "no matching function found";
const char* const CompileError::InvalidCustomInit = "invalid use of custom init";
const char* const CompileError::VarNotCustomInit  = "all member variables have to be initialized in custom init";
const char* const CompileError::VarNotInit        = "all member variables have to be initialized in constructor";
const char* const CompileError::MultipleInitStart = "member variable '";
const char* const CompileError::MultipleInitEnd   = "' is initialized more than once";
const char* const CompileError::SelfVarUseStart   = "member variable '";
const char* const CompileError::SelfVarUseEnd     = "' used before being initialized";

const char* const TokenError::UndefinedStart = "undefined token '";
const char* const TokenError::UndefinedEnd   = "'";

String SyntaxError::ExpectedAfterIn(const String& thing1, const String& thing2, const String& thing3) {
	return thing1 + " expected after " + thing2 + " in " + thing3;
}

String SyntaxError::ExpectedAfter(const String& thing1, const String& thing2) {
	return thing1 + " expected after " + thing2;
}

String SyntaxError::ExpectedAtEnd(const String& thing1, const String& thing2, const UInt line) {
	return thing1 + " expected at the end of " + thing2 + " at line " + String::ToString((Int)line);
}

String SyntaxError::EndExpected(const String& thing, const UInt line) {
	return "'end' expected to close " + thing + " at line " + String::ToString((Int)line);
}

String SyntaxError::Close(const String& thing, const String& close) {
	return close + " expected to close " + close;
}

String SyntaxError::Close(const String& thing, const String& close, const UInt line) {
	return close + " expected to close " + close + " at line " + String::ToString((Int)line);
}

String SyntaxError::MultipleAttribute(const String& attr) {
	return "multiple '" + attr + "' attributes";
}

String SyntaxError::AttributeIncompatible(const String& attr1, const String& attr2) {
	return "attribute '" + attr1 + "' is incompatible with '" + attr2 + "'";
}

const char* const SyntaxError::UnexpectedEOF        = "unexpected end of file";
const char* const SyntaxError::UnexpectedTokenStart = "unexpected token '";
const char* const SyntaxError::UnexpectedTokenEnd   = "'";
const char* const SyntaxError::MultipleDefaultExpr  = "multiple default cases in swith expression";
const char* const SyntaxError::MultipleDefault      = "multiple default cases in swith statement";
const char* const SyntaxError::ExprSwitchCase       = "expression expected in switch case";
const char* const SyntaxError::FewExpressions       = "too few expressions after '='";
const char* const SyntaxError::FewVariables         = "not enough variables in variable list";
const char* const SyntaxError::ArgNameExpected      = "argument name expected";
const char* const SyntaxError::BreakIntLow          = "integer argument for break is less than 1";
const char* const SyntaxError::ContinueIntLow       = "integer argument for continue is less than 1";
const char* const SyntaxError::AbortIntLow          = "integer argument for abort is less than 1";
const char* const SyntaxError::BreakLoops           = "not enough loops to break";
const char* const SyntaxError::ContinueLoops        = "not enough loops to continue";
const char* const SyntaxError::AbortLoops           = "not enough loops to abort";
const char* const SyntaxError::StructName           = "name expected for struct";
const char* const SyntaxError::DotName              = "name expected after '.'";
const char* const SyntaxError::CustomInitName       = "name expected in custom init";
const char* const SyntaxError::InvalidInclude       = "invalid name to include";

String SymbolError::Function(const Boxx::String& func, const Boxx::List<Boxx::String>& args) {
	if (args.IsEmpty()) {
		return SymbolError::FuncNotFoundStart + func + SymbolError::NotFoundEnd;
	}
	else {
		String argStr = args[0];

		for (UInt i = 1; i < args.Size(); i++) {
			argStr += ", " + args[i];
		}

		return SymbolError::FuncNotFoundStart + func + SymbolError::NotFoundArgs + argStr;
	}
}

const char* const SymbolError::ConstAssign       = "assignment to const value";
const char* const SymbolError::RedefinitionStart = "redefinition of symbol '";
const char* const SymbolError::RedefinitionEnd   = "'";
const char* const SymbolError::NotFoundStart     = "symbol '";
const char* const SymbolError::FuncNotFoundStart = "function '";
const char* const SymbolError::NotFoundEnd       = "' not found";
const char* const SymbolError::NotFoundArgs      = "' not found for arguments: ";
const char* const SymbolError::AmbiguousStart     = "'";
const char* const SymbolError::AmbiguousEnd       = "' is ambiguous";

String TypeError::Operator(const String& op, const String& type) {
	return "operator '" + op + "' for '" + type + "' not found";
}

String TypeError::Operator(const String& op, const String& type1, const String& type2) {
	return "operator '" + op + "' for '" + type1 + "' and '" + type2 + "' not found";
}

String TypeError::Assignment(const String& type1, const String& type2) {
	return "assigning '" + type1 + "' to '" + type2 + "'";
}

String TypeError::Dot(const String& var, const String& type) {
	return "'" + var + "' not found in '" + type + "'";
}

const char* const TypeError::IfExprType     = "if expression type error";
const char* const TypeError::SwitchExprType = "switch expression type error";

String InfoError::UpperName(const String& type, const String& name) {
	return type + " name: '" + name + "' does not start with an upper case character";
}

String InfoError::LowerName(const String& type, const String& name) {
	return type + " name: '" + name + "' does not start with a lower case character or underscore";
}

String InfoError::UpperUnderscoreName(const String& type, const String& name) {
	return "avoid underscores as separators in " + type + " name: '" + name + "' (use CamelCase instead)";
}

String InfoError::LowerUnderscoreName(const String& type, const String& name) {
	return "avoid underscores as separators in " + type + " name: '" + name + "' (use camelCase instead)";
}
