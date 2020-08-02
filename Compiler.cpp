#include "Boxx/List.h"
#include "Boxx/Map.h"
#include "Boxx/File.h"
#include "Boxx/Stack.h"
#include "Boxx/Time.h"
#include "Boxx/Tuple.h"
#include "Boxx/Mango.h"

#if BOXX_VERSION_LT(2, 1, 1)
	#error Boxx version of 2.1.1 or newer required
#endif

#include "Melon/Token.h"
#include "Melon/Lexer.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Nodes/RootNode.h"

#include "Melon/Optimizing/OptimizerInstruction.h"
#include "Melon/Optimizing/KiwiOptimizer.h"

#include "Melon/Symbols/Symbols.h"

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"

using namespace Boxx;

using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

int main() {
	Time::Start();
	
	ErrorLog::compileLogger.Info("compiling started");
	ErrorLog::compileLogger.Log("setup started");

	List<TokenPattern> patterns;
	// Comments
	patterns.Add(TokenPattern(TokenType::CommentLine, "%-%-~\n*", true, true));
	patterns.Add(TokenPattern(TokenType::CommentBlock, "%-#%/+./%1%-", true, true));

	// Raw
	patterns.Add(TokenPattern(TokenType::Float, "%d*%.%d+"));
	patterns.Add(TokenPattern(TokenType::Integer, "%d+"));
	patterns.Add(TokenPattern(TokenType::Boolean, "true|false"));

	// Bitwise
	patterns.Add(TokenPattern(TokenType::BNand, "%~%&", true));
	patterns.Add(TokenPattern(TokenType::BNor, "%~%|", true));
	patterns.Add(TokenPattern(TokenType::BXnor, "%~%^", true));
	patterns.Add(TokenPattern(TokenType::BAnd, "%&", true));
	patterns.Add(TokenPattern(TokenType::BOr, "%|", true));
	patterns.Add(TokenPattern(TokenType::BXor, "%^", true));
	patterns.Add(TokenPattern(TokenType::BShiftLeft, "%<%<", true));
	patterns.Add(TokenPattern(TokenType::BShiftRight, "%>%>", true));

	// Compare
	patterns.Add(TokenPattern(TokenType::Equal, "%=%=", true));
	patterns.Add(TokenPattern(TokenType::NotEqual, "%~%=", true));
	patterns.Add(TokenPattern(TokenType::LessEq, "%<%=", true));
	patterns.Add(TokenPattern(TokenType::GreaterEq, "%>%=", true));
	patterns.Add(TokenPattern(TokenType::Less, "%<", true));
	patterns.Add(TokenPattern(TokenType::Greater, "%>", true));

	patterns.Add(TokenPattern(TokenType::BNot, "%~", true));

	// Math
	patterns.Add(TokenPattern(TokenType::Plus, "%+", true));
	patterns.Add(TokenPattern(TokenType::Minus, "%-", true));
	patterns.Add(TokenPattern(TokenType::Mul, "%*", true));
	patterns.Add(TokenPattern(TokenType::IDiv, "%/%/", true));
	patterns.Add(TokenPattern(TokenType::Div, "%/", true));
	patterns.Add(TokenPattern(TokenType::Mod, "%%", true));
	//patterns.Add(TokenPattern(TokenType::Pow, "%^", true));

	// Assign
	patterns.Add(TokenPattern(TokenType::Assign, "%=", true));

	// Delimiters
	patterns.Add(TokenPattern(TokenType::ParenOpen, "%(", true));
	patterns.Add(TokenPattern(TokenType::ParenClose, "%)", true));
	patterns.Add(TokenPattern(TokenType::CurlyOpen, "%{", true));
	patterns.Add(TokenPattern(TokenType::CurlyClose, "%}", true));
	patterns.Add(TokenPattern(TokenType::SquareOpen, "%[", true));
	patterns.Add(TokenPattern(TokenType::SquareClose, "%]", true));

	// Operators
	patterns.Add(TokenPattern(TokenType::Comma, "%,", true));
	patterns.Add(TokenPattern(TokenType::Colon, "%:", true));
	patterns.Add(TokenPattern(TokenType::Dot, "%.", true));
	patterns.Add(TokenPattern(TokenType::Len, "%#", true));

	// Keywords
	patterns.Add(TokenPattern(TokenType::If, "if"));
	patterns.Add(TokenPattern(TokenType::Then, "then"));
	patterns.Add(TokenPattern(TokenType::AlsoIf, "alsoif"));
	patterns.Add(TokenPattern(TokenType::ElseIf, "elseif"));
	patterns.Add(TokenPattern(TokenType::While, "while"));
	patterns.Add(TokenPattern(TokenType::AlsoWhile, "alsowhile"));
	patterns.Add(TokenPattern(TokenType::ElseWhile, "elsewhile"));
	patterns.Add(TokenPattern(TokenType::Do, "do"));
	patterns.Add(TokenPattern(TokenType::Also, "also"));
	patterns.Add(TokenPattern(TokenType::Else, "else"));
	patterns.Add(TokenPattern(TokenType::End, "end"));
	patterns.Add(TokenPattern(TokenType::Switch, "switch"));
	patterns.Add(TokenPattern(TokenType::Case, "case"));
	patterns.Add(TokenPattern(TokenType::Default, "default"));
	patterns.Add(TokenPattern(TokenType::Function, "function"));
	patterns.Add(TokenPattern(TokenType::Return, "return"));
	patterns.Add(TokenPattern(TokenType::Break, "break"));
	patterns.Add(TokenPattern(TokenType::Abort, "abort"));
	patterns.Add(TokenPattern(TokenType::Continue, "continue"));
	patterns.Add(TokenPattern(TokenType::Struct, "struct"));
	patterns.Add(TokenPattern(TokenType::Init, "init"));

	patterns.Add(TokenPattern(TokenType::Const, "const"));
	patterns.Add(TokenPattern(TokenType::Ref, "ref"));
	patterns.Add(TokenPattern(TokenType::Copy, "copy"));
	patterns.Add(TokenPattern(TokenType::Nil, "nil"));

	patterns.Add(TokenPattern(TokenType::Static, "static"));
	patterns.Add(TokenPattern(TokenType::Override, "override"));
	patterns.Add(TokenPattern(TokenType::Operator, "operator"));
	patterns.Add(TokenPattern(TokenType::Required, "required"));
	patterns.Add(TokenPattern(TokenType::Debug, "debug"));

	// Logic
	patterns.Add(TokenPattern(TokenType::Not, "not"));
	patterns.Add(TokenPattern(TokenType::Or, "or"));
	patterns.Add(TokenPattern(TokenType::And, "and"));
	patterns.Add(TokenPattern(TokenType::Xor, "xor"));
	patterns.Add(TokenPattern(TokenType::Nor, "nor"));
	patterns.Add(TokenPattern(TokenType::Nand, "nand"));
	patterns.Add(TokenPattern(TokenType::Xnor, "xnor"));

	// Name
	patterns.Add(TokenPattern(TokenType::Name, "%w+"));

	const String filename = "tests/test.melon";

	const String code = FileReader(filename).ReadAll();

	Symbol::Setup();

	float time = Time::Total();
	float dt = time;

	ErrorLog::compileLogger.Log("setup finished in: " + String::ToString(dt) + " seconds");
	
	try {
		ErrorLog::compileLogger.Log("lexing started");

		List<Token> tokens = Lexer::Lex(patterns, code, filename);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("setup finished in: " + String::ToString(dt) + " seconds");

		ErrorLog::compileLogger.Log("parsing started");
		RootNode node = Parser::Parse(tokens, filename);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("parsing finished in: " + String::ToString(dt) + " seconds");

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		ErrorLog::compileLogger.Log("scanning started");
		node.Scan();

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("scanning finished in: " + String::ToString(dt) + " seconds");
		
		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		ErrorLog::compileLogger.Log("compiling nodes started");
		List<OptimizerInstruction> instructions = node.Compile();

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("compiling nodes finished in: " + String::ToString(dt) + " seconds");

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		ErrorLog::compileLogger.Log("optimizing kiwi");
		List<Instruction> optimizedInstructions = KiwiOptimizer::Optimize(instructions, 1);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("kiwi optimizing finished in: " + String::ToString(dt) + " seconds");

		ErrorLog::compileLogger.Log("writing symbol table");

		FileWriter symbols = FileWriter("tests/symbols.mango");
		symbols.Write(Mango::Encode(Symbol::ToMango(true), true));
		symbols.Close();

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("symbol table finished in: " + String::ToString(dt) + " seconds");

		ErrorLog::compileLogger.Log("writing ast");

		FileWriter ast = FileWriter("tests/ast.mango");
		ast.Write(node.ToString());
		ast.Close();

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("ast finished in: " + String::ToString(dt) + " seconds");

		// ------------- Kiwi ---------------

		ErrorLog::compileLogger.Log("writing kiwi");

		KiwiLang::WriteToFile("tests/assembly.kiwi", ErrorLog::logger, optimizedInstructions);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("kiwi finished in: " + String::ToString(dt) + " seconds");

		bool error = false;

		for (const Tuple<Logger::LogLevel, String>& err : ErrorLog::logger) {
			if (err.value1 == Logger::LogLevel::Error) {
				error = true;
				break;
			}
		}

		if (error) throw CompileError("", FileInfo());

		// ------------ Intel -------------

		ErrorLog::compileLogger.Log("writing asm");

		Pointer<x86_64Converter> intelConverter = new x86_64Converter(x86_64Syntax::Intel, ErrorLog::logger);
		KiwiLang::WriteToFile("tests/assembly.asm", intelConverter, optimizedInstructions);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("asm finished in: " + String::ToString(dt) + " seconds");

		// ------------ AT&T --------------

		ErrorLog::compileLogger.Log("writing s");

		Pointer<x86_64Converter> atntConverter = new x86_64Converter(x86_64Syntax::ATnT, ErrorLog::logger);
		KiwiLang::WriteToFile("tests/assembly.s", atntConverter, optimizedInstructions);

		dt = time;
		time = Time::Total();
		dt = time - dt;

		ErrorLog::compileLogger.Log("s finished in: " + String::ToString(dt) + " seconds");

		error = false;

		for (const Tuple<Logger::LogLevel, String>& err : ErrorLog::logger) {
			if (err.value1 == Logger::LogLevel::Error) {
				error = true;
				break;
			}
		}

		if (error) throw CompileError("", FileInfo());

		ErrorLog::LogErrors();
	}
	//*
	catch (CompileError e) {
		ErrorLog::LogErrors();
		time = Time::Total();

		try {
			ErrorLog::compileLogger.Fatal("compiling interrupted after: " + String::ToString(time) + " seconds");
		}
		catch (FatalLoggerError e) {
			return 0;
		}
	}//*/
	catch (int e) {
		Int i = 0;
	}

	time = Time::Total();
	ErrorLog::compileLogger.Info("compiling finished in: " + String::ToString(time) + " seconds");

	return 0;
}
