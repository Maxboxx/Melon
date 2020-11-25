#pragma once

#include "Boxx/Error.h"
#include "Boxx/String.h"
#include "Boxx/Logger.h"
#include "Boxx/Tuple.h"
#include "Boxx/Pair.h"
#include "Boxx/List.h"
#include "Boxx/Set.h"

#include "Melon/Symbols/ScopeList.h"

namespace Melon {
	namespace Symbols {
		class Symbols;
	}

	///B ErrorLog
	/// Used to log errors
	class ErrorLog {
	public:
		///T Log
		static void Log(const class CompileError& error);

		///T Info
		static void Info(const class CompileError& error);

		///T Success
		static void Success(const class CompileError& error);

		///T Warning
		static void Warning(const class CompileError& error);

		///T Error
		static void Error(const class CompileError& error);

		///T Fatal
		static void Fatal(const class CompileError& error);

		///T Add Marker
		static void AddMarker();

		///T Remove Marker
		static void RemoveMarker();

		///T Revert To Marker
		static void RevertToMarker();

		///T Revert and Remove mark
		static void Revert();

		///T Log Errors
		static void LogErrors();

		///T Has Errors
		static bool HasError();

		///T Error Count
		static Boxx::UInt ErrorCount();

		///T Stop On Error
		static bool stopOnError;

		///T Loggers
		///M
		static Boxx::Logger compileLogger;
		static Boxx::Logger logger;
		///M

	private:
		enum class LogLevel {
			Log,
			Info,
			Success,
			Warning,
			Error,
			Fatal
		};

		static Boxx::List<Boxx::Tuple<LogLevel, class CompileError>> errors;
		static Boxx::List<Boxx::UInt> markers;
	};

	///B FileInfo
	/// Contains info about a location in a file
	struct FileInfo {
		///T Filename
		Boxx::String filename;

		///T Line
		Boxx::UInt line;

		///T Current Namespace
		Symbols::ScopeList currentNamespace;

		///T File Scope
		Boxx::String fileScope;

		///T Included Namespaces
		Boxx::Set<Symbols::ScopeList> includedNamespaces;

		///T Statement number
		/// The statement number of this file location
		Boxx::UInt statement;

		///T Empty constructor
		FileInfo() {
			filename = "";
			line = 0;
			statement = 0;
		}

		///T Constructor
		FileInfo(const Boxx::String& filename, const Boxx::UInt line, const Boxx::UInt statementNumber) {
			this->filename = filename;
			this->line = line;
			this->statement = statementNumber;
		}

		///T Constructor with Namespaces
		FileInfo(const Boxx::String& filename, const Boxx::UInt line, const Boxx::UInt statementNumber, const Symbols::ScopeList& currentNamespace, const Boxx::Set<Symbols::ScopeList>& includedNamespaces) {
			this->filename = filename;
			this->line = line;
			this->statement = statementNumber;
			this->currentNamespace = currentNamespace;
			this->includedNamespaces = includedNamespaces;
		}
	};

	///B CompileError
	/// Base class for all errors
	class CompileError : public Boxx::Error {
	public:
		///T Constructor
		///A msg: The error message
		///A file: The filename of the file that contains the error
		///A line: The line number of the error
		CompileError(const char* const msg, const FileInfo& file) : Boxx::Error(msg) {
			this->file = file;
			type = ErrorType::Compile;
		}

		CompileError() {}

		///T Message
		/// Returns the error message
		Boxx::String Message() const {
			if (file.line == 0) {
				return what();
			}
			else {
				return file.filename + ":" + Boxx::String::ToString((Boxx::Int)file.line) + ": " + what();
			}
		}

		virtual Boxx::String Name() const override {
			return "CompileError";
		}

		///T Func Not Return
		static Boxx::String FuncNotReturn(const Symbols::Symbols& func);

		///T Return
		static Boxx::String Return(const Boxx::UInt funcRetCount, const Boxx::UInt args);

		///T Errors
		/// All compile error messages
		///M
		static const char* const SelfInit;
		static const char* const InvalidRef;
		static const char* const FuncNotFound;
		static const char* const InvalidCustomInit;
		static const char* const VarNotCustomInitStart;
		static const char* const VarNotCustomInitEnd;
		static const char* const VarNotInitStart;
		static const char* const VarNotInitEnd;
		static const char* const MultipleInitStart;
		static const char* const MultipleInitEnd;
		static const char* const SelfVarUseStart;
		static const char* const SelfVarUseEnd;
		static const char* const InvalidNoRef;
		///M

		enum class ErrorType : Boxx::UByte {
			Compile,
			Token,
			Syntax,
			Symbol,
			Type,
			Warning,
			Info,
			Plain
		};

		ErrorType type;
		FileInfo file;
	};

	///B TokenError
	/// Thrown for invalid tokens
	class TokenError : public CompileError {
	public:
		///T Constructor
		TokenError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Token;
		}

		virtual Boxx::String Name() const override {
			return "TokenError";
		}

		///T Errors
		/// All token error messages
		///M
		static const char* const UndefinedStart;
		static const char* const UndefinedEnd;
		///M
	};

	///B SyntaxError
	/// Thrown for syntax errors
	class SyntaxError : public CompileError {
	public:
		///T Constructor
		SyntaxError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Syntax;
		}

		virtual Boxx::String Name() const override {
			return "SyntaxError";
		}

		static Boxx::String ExpectedAfterIn(const Boxx::String& expected, const Boxx::String& after, const Boxx::String& in);
		static Boxx::String ExpectedAfter(const Boxx::String& expected, const Boxx::String& after);
		static Boxx::String ExpectedAtEnd(const Boxx::String& expected, const Boxx::String& atEndOf, const Boxx::UInt line);
		static Boxx::String EndExpected(const Boxx::String& thing, const Boxx::UInt line);
		static Boxx::String Close(const Boxx::String& thing, const Boxx::String& close);
		static Boxx::String Close(const Boxx::String& thing, const Boxx::String& close, const Boxx::UInt line);
		static Boxx::String MultipleAttribute(const Boxx::String& attr);
		static Boxx::String AttributeIncompatible(const Boxx::String& attr1, const Boxx::String& attr2);

		///T Errors
		/// All syntax error messages
		///M
		static const char* const UnexpectedEOF;
		static const char* const UnexpectedTokenStart;
		static const char* const UnexpectedTokenEnd;
		static const char* const MultipleDefaultExpr;
		static const char* const MultipleDefault;
		static const char* const DefaultRequired;
		static const char* const ExprSwitchCase;
		static const char* const ManyExprAssign;
		static const char* const FewVariables;
		static const char* const ArgNameExpected;
		static const char* const BreakIntLow;
		static const char* const ContinueIntLow;
		static const char* const AbortIntLow;
		static const char* const BreakLoops;
		static const char* const BreakScopes;
		static const char* const ContinueLoops;
		static const char* const AbortLoops;
		static const char* const StructName;
		static const char* const EnumName;
		static const char* const DotName;
		static const char* const CustomInitName;
		static const char* const InvalidInclude;
		static const char* const InvalidRef;
		///M
	};

	///B SymbolError
	/// Thrown for errors with symbols
	class SymbolError : public CompileError {
	public:
		///T Constructor
		SymbolError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Symbol;
		}

		virtual Boxx::String Name() const override {
			return "SymbolError";
		}

		static Boxx::String Function(const Boxx::String& func, const Boxx::List<Boxx::String>& args);
		static Boxx::String RecursiveStruct(const Boxx::String& structName);
		static Boxx::String Ambiguous(const Boxx::String& symbol);
		static Boxx::String AmbiguousCall(const Boxx::String& func, const Boxx::List<Boxx::String>& args);
		static Boxx::String ImplicitConvert(const Boxx::String& from, const Boxx::String& to);
		static Boxx::String ExplicitConvert(const Boxx::String& from, const Boxx::String& to);

		///T Errors
		/// All syntax error messages
		///M
		static const char* const ConstAssign;
		static const char* const RedefinitionStart;
		static const char* const RedefinitionEnd;
		static const char* const NotFoundStart;
		static const char* const FuncNotFoundStart;
		static const char* const NotFoundEnd;
		static const char* const NotFoundArgs;
		///M
	};

	///B TypeError
	/// Thrown for type mismatches
	class TypeError : public CompileError {
	public:
		///T Constructor
		TypeError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Type;
		}

		virtual Boxx::String Name() const override {
			return "TypeError";
		}

		static Boxx::String Operator(const Boxx::String& op, const Boxx::String& type);
		static Boxx::String Operator(const Boxx::String& op, const Boxx::String& type1, const Boxx::String& type2);
		static Boxx::String Assignment(const Boxx::String& type1, const Boxx::String& type2);
		static Boxx::String ConditionAssignment(const Boxx::String& type);
		static Boxx::String Dot(const Boxx::String& var, const Boxx::String& type);

		///T Errors
		/// All type error messages
		///M
		static const char* const IfExprType;
		static const char* const SwitchExprType;
		static const char* const DefaultType;
		///M
	};

	///B WarningError
	class WarningError : public CompileError {
	public:
		WarningError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Warning;
		}

		virtual Boxx::String Name() const override {
			return "WarningError";
		}

		static Boxx::String NoRefArg(const Boxx::String& func, const Boxx::UInt argIndex);
	};

	///B InfoError
	class InfoError : public CompileError {
	public:
		InfoError(const char* const msg, const FileInfo& file) : CompileError(msg, file) {
			type = ErrorType::Info;
		}

		virtual Boxx::String Name() const override {
			return "InfoError";
		}

		static Boxx::String UpperName(const Boxx::String& type, const Boxx::String& name);
		static Boxx::String LowerName(const Boxx::String& type, const Boxx::String& name);
		static Boxx::String UpperUnderscoreName(const Boxx::String& type, const Boxx::String& name);
		static Boxx::String LowerUnderscoreName(const Boxx::String& type, const Boxx::String& name);
	};

	///B PlainError
	class PlainError : public CompileError {
	public:
		PlainError(const char* const msg) : CompileError(msg, FileInfo()) {
			type = ErrorType::Plain;
		}

		virtual Boxx::String Name() const override {
			return "PlainError";
		}
	};
}