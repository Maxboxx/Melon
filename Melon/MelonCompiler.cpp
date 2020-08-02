#include "MelonCompiler.h"

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/Token.h"
#include "Melon/Lexer.h"

#include "Melon/Nodes/RootNode.h"

#include "Melon/Symbols/Symbols.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"
#include "Melon/Optimizing/KiwiOptimizer.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

void MelonCompiler::Compile(const String& mainMelonFile, const CompilerOptions& options) {
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

	const String filename = mainMelonFile;
	
	CompilerOptions compOptions = options;
	compOptions.includeDirectories = List<String>();

	for (const String& dir : options.includeDirectories) {
		compOptions.includeDirectories.Add(dir);
	}

	compOptions.includeDirectories.Add(Regex("^(~[%/\\]*){[%/\\].*}?$").Match(filename)[0]);

	if (compOptions.outputName.Size() == 0) {
		compOptions.outputName = Regex("(~[%/\\]-){%.~[%/\\%.]*}?$").Match(filename)[0];
	}

	for (UInt i = 0; i < compOptions.includeDirectories.Size(); i++) {
		const String dir = compOptions.includeDirectories[i];

		if (dir[dir.Size() - 1] != '/' && dir[dir.Size() - 1] != '\\') {
			compOptions.includeDirectories[i] += "/";
		}
	}

	if (compOptions.outputDirectory.Size() == 0) {
		compOptions.outputDirectory = compOptions.includeDirectories.Last();
	}
	else if (compOptions.outputDirectory[compOptions.outputDirectory.Size() - 1] != '/' && compOptions.outputDirectory[compOptions.outputDirectory.Size() - 1] != '\\') {
		compOptions.outputDirectory += "/";
	}

	const String code = FileReader(filename).ReadAll();

	Symbol::Setup();

	try {
		List<Token> tokens = Lexer::Lex(patterns, code, filename);
		RootNode node = Parser::Parse(tokens, filename);

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		node.Scan();

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		List<OptimizerInstruction> instructions = node.Compile();

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		List<Instruction> optimizedInstructions = KiwiOptimizer::Optimize(instructions, compOptions.kiwiOptimizationPasses);

		if (compOptions.outputSymbols) {
			FileWriter symbols = FileWriter(compOptions.outputDirectory + compOptions.outputName + "_symbols.mango");
			symbols.Write(Mango::Encode(Symbol::ToMango(true), true));
			symbols.Close();
		}

		if (compOptions.outputAST) {
			FileWriter ast = FileWriter(compOptions.outputDirectory + compOptions.outputName + "_ast.mango");
			ast.Write(node.ToString());
			ast.Close();
		}

		// ------------- Kiwi ---------------

		if (compOptions.outputKiwi) {
			KiwiLang::WriteToFile(compOptions.outputDirectory + compOptions.outputName + ".kiwi", ErrorLog::logger, optimizedInstructions);
		}

		bool error = false;

		for (const Tuple<Logger::LogLevel, String>& err : ErrorLog::logger) {
			if (err.value1 == Logger::LogLevel::Error) {
				error = true;
				break;
			}
		}

		if (error) throw CompileError("", FileInfo());

		// ------------ Converter -------------

		if (compOptions.outputAssembly) {
			KiwiLang::WriteToFile(compOptions.outputDirectory + compOptions.outputName + ".asm", compOptions.converter, optimizedInstructions);
		}

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
	}//*/
	catch (int e) {
		Int i = 0;
	}
}

void MelonCompiler::CompileFile(const String& filename, const CompilerOptions& options) {
	
}