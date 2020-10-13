#include "Errors.h"

#include "Boxx/Map.h"
#include "Boxx/Console.h"

#include "Melon/Symbols/Symbols.h"

using namespace Boxx;
using namespace Melon;
using namespace Melon::Symbols;

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

void ErrorLog::Success(const CompileError& error) {
	errors.Add(Tuple<LogLevel, CompileError>(LogLevel::Success, error));
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
	throw FatalLoggerError();
}

void ErrorLog::AddMarker() {
	markers.Add(errors.Size());
}

void ErrorLog::RemoveMarker() {
	markers.RemoveAt(markers.Size() - 1);
}

void ErrorLog::RevertToMarker() {
	if (markers.Last() < errors.Size()) {
		errors.RemoveAt(markers.Last(), errors.Size() - markers.Last());
	}
}

void ErrorLog::Revert() {
	RevertToMarker();
	RemoveMarker();
}

void ErrorLog::LogErrors() {
	Map<String, List<Tuple<LogLevel, CompileError>>> sortedErrors;
	List<Tuple<LogLevel, CompileError>> noFileErrors;

	for (const Tuple<LogLevel, CompileError>& error : errors) {
		List<Tuple<LogLevel, CompileError>> errorList;

		if (error.value2.file.line == 0) {
			noFileErrors.Add(error);
		}
		else if (sortedErrors.Contains(error.value2.file.filename, errorList)) {
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
			switch (error.value1) {
				case LogLevel::Log:     logger.Log(error.value2.Message()); break;
				case LogLevel::Info:    logger.Info(error.value2.Message()); break;
				case LogLevel::Success: logger.Success(error.value2.Message()); break;
				case LogLevel::Warning: logger.Warning(error.value2.Message()); break;
				case LogLevel::Error:   logger.Error(error.value2.Message()); break;
				case LogLevel::Fatal:   {
					logger.Write("fatal: " + error.value2.Message());
					Console::Print("fatal: " + error.value2.Message());
					break;
				}
			}
		}
	}

	for (const Tuple<LogLevel, CompileError>& error : noFileErrors) {
		switch (error.value1) {
			case LogLevel::Log:     logger.Log(error.value2.Message()); break;
			case LogLevel::Info:    logger.Info(error.value2.Message()); break;
			case LogLevel::Success: logger.Success(error.value2.Message()); break;
			case LogLevel::Warning: logger.Warning(error.value2.Message()); break;
			case LogLevel::Error:   logger.Error(error.value2.Message()); break;
			case LogLevel::Fatal:   {
				logger.Write("fatal: " + error.value2.Message());
				Console::Print("fatal: " + error.value2.Message());
				break;
			}
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

UInt ErrorLog::ErrorCount() {
	UInt count = 0;

	for (const Tuple<LogLevel, CompileError>& error : errors) {
		if (error.value1 == LogLevel::Error) {
			count++;
		}
	}

	return count;
}

String CompileError::FuncNotReturn(const Symbol& func) {
	if (func.returnValues.Size() > 1) {
		return "function '" + func.scope.ToString() + "' must return values";
	}
	else {
		return "function '" + func.scope.ToString() + "' must return a value";
	}
}

String CompileError::Return(const UInt funcRetCount, const UInt args) {
	if (args > funcRetCount) {
		return "too many arguments to return";
	}
	else {
		return "not enough arguments to return";
	}
}

const char* const CompileError::SelfInit              = "'self' used before all member variables are initialized";
const char* const CompileError::InvalidRef            = "invalid reference value";
const char* const CompileError::FuncNotFound          = "no matching function found";
const char* const CompileError::InvalidCustomInit     = "invalid use of custom init";
const char* const CompileError::VarNotCustomInitStart = "member variable '";
const char* const CompileError::VarNotCustomInitEnd   = "' has to be initialized in custom init";
const char* const CompileError::VarNotInitStart       = "member variable '";
const char* const CompileError::VarNotInitEnd         = "' has to be initialized in constructor";
const char* const CompileError::MultipleInitStart     = "member variable '";
const char* const CompileError::MultipleInitEnd       = "' is initialized more than once";
const char* const CompileError::SelfVarUseStart       = "member variable '";
const char* const CompileError::SelfVarUseEnd         = "' used before being initialized";
const char* const CompileError::InvalidNoRef          = "invalid use of 'noref'";

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
const char* const SyntaxError::DefaultRequired      = "default case required in switch expression";
const char* const SyntaxError::ExprSwitchCase       = "expression expected in switch case";
const char* const SyntaxError::FewExpressions       = "too few expressions after '='";
const char* const SyntaxError::FewVariables         = "not enough variables in variable list";
const char* const SyntaxError::ArgNameExpected      = "argument name expected";
const char* const SyntaxError::BreakIntLow          = "integer argument for break is less than 1";
const char* const SyntaxError::ContinueIntLow       = "integer argument for continue is less than 1";
const char* const SyntaxError::AbortIntLow          = "integer argument for abort is less than 1";
const char* const SyntaxError::BreakLoops           = "not enough loops to break";
const char* const SyntaxError::BreakScopes          = "not enough scopes to break";
const char* const SyntaxError::ContinueLoops        = "not enough loops to continue";
const char* const SyntaxError::AbortLoops           = "not enough loops to abort";
const char* const SyntaxError::StructName           = "name expected for struct";
const char* const SyntaxError::EnumName             = "name expected for enum";
const char* const SyntaxError::DotName              = "name expected after '.'";
const char* const SyntaxError::CustomInitName       = "name expected in custom init";
const char* const SyntaxError::InvalidInclude       = "invalid name to include";
const char* const SyntaxError::InvalidRef           = "'ref' can only be used for function arguments";

String SymbolError::Function(const String& func, const List<String>& args) {
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

String SymbolError::RecursiveStruct(const String& structName) {
	return "struct '" + structName + "' is recursive";
}

String SymbolError::Ambiguous(const String& symbol) {
	return "'" + symbol + "' is ambiguous";
}

String SymbolError::AmbiguousCall(const String& func, const List<String>& args) {
	if (args.IsEmpty()) {
		return "call to '" + func + "' is ambiguous";
	}
	else {
		String argStr = args[0];

		for (UInt i = 1; i < args.Size(); i++) {
			argStr += ", " + args[i];
		}

		return "call to '" + func + "' is ambiguous for arguments: " + argStr;
	}
}

String SymbolError::ImplicitConvert(const String& from, const String& to) {
	return "implicit conversion from '" + from + "' to '" + to + "' not found";
}

String SymbolError::ExplicitConvert(const String& from, const String& to) {
	return "conversion from '" + from + "' to '" + to + "' not found";
}

const char* const SymbolError::ConstAssign       = "assignment to const value";
const char* const SymbolError::RedefinitionStart = "redefinition of symbol '";
const char* const SymbolError::RedefinitionEnd   = "'";
const char* const SymbolError::NotFoundStart     = "symbol '";
const char* const SymbolError::FuncNotFoundStart = "function '";
const char* const SymbolError::NotFoundEnd       = "' not found";
const char* const SymbolError::NotFoundArgs      = "' not found for arguments: ";

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
const char* const TypeError::DefaultType    = "default expression type error";

String WarningError::NoRefArg(const String& func, const UInt argIndex) {
	return "argument number " + String::ToString(argIndex + 1) + " to '" + func + "' will not be directly referenced";
}

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