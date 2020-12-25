#include "MelonCompiler.h"

#include "Boxx/File.h"
#include "Boxx/Mango.h"
#include "Boxx/System.h"

#include "Kiwi/Kiwi.h"
#include "Kiwi/x86_64Converter.h"

#include "Melon/Errors.h"
#include "Melon/Token.h"
#include "Melon/Lexer.h"

#include "Melon/Nodes/RootNode.h"

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

CompilerOptions CompilerOptions::LoadFromFile(const String& mangoFile) {
	FileReader file = FileReader(mangoFile);
	Mango mango;
	
	try {
		mango = Mango::Decode(file.ReadAll());
	}
	catch (MangoDecodeError e) {
		ErrorLog::Fatal(PlainError(mangoFile + ": " + e.Message()));
	}

	file.Close();

	// TODO: Errors for incorrect format
	CompilerOptions options;
	Map<String, Mango> optionsMap = mango;

	if (optionsMap.Contains("main")) {
		options.mainFile = optionsMap["main"];
	}

	if (optionsMap.Contains("name")) {
		options.outputName = optionsMap["name"];
	}

	if (optionsMap.Contains("include")) {
		List<Mango> includeDirs = optionsMap["include"];
		
		for (const Mango& dir : includeDirs) {
			if (dir.GetLabel() == "dir") {
				options.includeDirectories.Add(dir);
			}
		}
	}

	if (optionsMap.Contains("output")) {
		Map<String, Mango> outputOptions = optionsMap["output"];

		if (outputOptions.Contains("dir")) {
			options.outputDirectory = outputOptions["dir"];
		}

		if (outputOptions.Contains("ast")) {
			options.outputAST = outputOptions["ast"];
		}

		if (outputOptions.Contains("symbols")) {
			options.outputSymbols = outputOptions["symbols"];
		}

		if (outputOptions.Contains("melon")) {
			options.outputMelon = outputOptions["melon"];
		}

		if (outputOptions.Contains("kiwi")) {
			options.outputKiwi = outputOptions["kiwi"];
		}

		if (outputOptions.Contains("assembly")) {
			options.outputAssembly = outputOptions["assembly"];
		}

		if (outputOptions.Contains("executable")) {
			options.outputExecutable = outputOptions["executable"];
		}
	}

	if (optionsMap.Contains("optimizing")) {
		Mango optimizing = optionsMap["optimizing"];

		if (optimizing.Type() == MangoType::Map) {
			Map<String, Mango> opti = optionsMap["optimizing"];

			if (opti.Contains("ast")) {
				if (opti["ast"].Type() == MangoType::Number) {
					options.astOptimizationPasses = (double)opti["ast"];
				}
				else {
					options.astOptimizationPasses = (bool)opti["ast"] ? Math::UIntMax() : 0;
				}
			}

			if (opti.Contains("kiwi")) {
				if (opti["kiwi"].Type() == MangoType::Number) {
					options.kiwiOptimizationPasses = (double)opti["kiwi"];
				}
				else {
					options.kiwiOptimizationPasses = (bool)opti["kiwi"] ? Math::UIntMax() : 0;
				}
			}
		}
		else if (optimizing.Type() == MangoType::Number) {
			double passes = optimizing;
			options.astOptimizationPasses = passes;
			options.kiwiOptimizationPasses = passes;
		}
		else if (optimizing.Type() == MangoType::Boolean) {
			UInt passes = (bool)optimizing ? Math::UIntMax() : 0;
			options.astOptimizationPasses = passes;
			options.kiwiOptimizationPasses = passes;
		}
	}

	return options;
}

void MelonCompiler::Compile(const CompilerOptions& options) {
	const String filename = options.mainFile;
	
	CompilerOptions compOptions = options;
	compOptions.includeDirectories = List<String>();

	for (const String& dir : options.includeDirectories) {
		compOptions.includeDirectories.Add(dir);
	}

	compOptions.includeDirectories.Add(Regex::Match("^(~[%/\\]*){[%/\\].*}?$", filename).Get().match);

	if (compOptions.outputName.Size() == 0) {
		compOptions.outputName = Regex::Match("(~[%/\\]-){%.~[%/\\%.]*}?$", filename).Get().match;
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

	{
		const String dir = compOptions.outputDirectory.Sub(0, compOptions.outputDirectory.Size() - 2);

		if (System::DirectoryExists(dir)) {
			System::DeleteDirectory(dir);
		}

		if (!System::DirectoryExists(dir)) {
			System::CreateDirectory(dir);
		}
	}

	const String code = FileReader(filename).ReadAll();

	SymbolTable::Setup();

	try {
		ParsingInfo info = Parser::Parse(filename, compOptions);

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		//ErrorLog::AddMarker();
		info.root.IncludeScan(info);
		//ErrorLog::Revert();

		/*
		info.root.parsingInfo = &info;

		Node::root = &info.root;

		ScanInfoStack scanInfo;

		for (UInt i = 0; i < (ULong)compOptions.astOptimizationPasses + 1; i++) {
			if (i > 0) {
				OptimizeInfo opInfo;
				opInfo.usedVariables = scanInfo.usedVariables;
				opInfo.usedTypes     = scanInfo.usedTypes;
				opInfo.usedFunctions = scanInfo.usedFunctions;
				info.root.Optimize(opInfo);

				if (ErrorLog::HasError()) {
					throw CompileError("", FileInfo());
				}

				if (!opInfo.optimized) break;

				scanInfo = ScanInfoStack();
			}

			scanInfo = info.root.Scan();

			if (ErrorLog::HasError()) {
				throw CompileError("", FileInfo());
			}
		}

		List<OptimizerInstruction> instructions = info.root.Compile(scanInfo.usedVariables);

		if (ErrorLog::HasError()) {
			throw CompileError("", FileInfo());
		}

		List<Instruction> optimizedInstructions = KiwiOptimizer::Optimize(instructions, compOptions.kiwiOptimizationPasses);

		if (compOptions.outputMelon) {
			info.root.ToMelonFiles(compOptions);
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
		*/

		ErrorLog::Success(PlainError("compilation successful"));
		ErrorLog::LogErrors();
	}
	catch (CompileError e) {
		ErrorLog::Error(PlainError("compilation failed"));
		ErrorLog::LogErrors();
	}
}

void MelonCompiler::CompileFile(const String& filename, const CompilerOptions& options) {
	
}