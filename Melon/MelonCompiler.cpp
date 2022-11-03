#include "MelonCompiler.h"

#include "Boxx/File.h"
#include "Boxx/Mango.h"
#include "Boxx/System.h"
#include "Boxx/Path.h"

#include "Kiwi/Old/Kiwi.h"
#include "Kiwi/Old/x86_64Converter.h"
#include "Kiwi/KiwiProgram.h"

#include "Melon/Errors.h"
#include "Melon/Token.h"
#include "Melon/Lexer.h"

#include "Melon/Nodes/RootNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"
#include "Melon/Optimizing/KiwiOptimizer.h"

using namespace Boxx;
using namespace KiwiOld;

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
		ErrorLog::Fatal(LogMessage::Message(mangoFile + ": " + e.Message()), FileInfo());
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

	// Load include options
	if (optionsMap.Contains("include")) {
		List<Mango> includeDirs = optionsMap["include"];
		
		for (const Mango& dir : includeDirs) {
			if (dir.GetLabel() == "dir") {
				options.includeDirectories.Add(dir);
			}
		}
	}

	// Load output options
	if (optionsMap.Contains("output")) {
		Map<String, Mango> outputOptions = optionsMap["output"];

		if (outputOptions.Contains("dir")) {
			options.outputDirectory = outputOptions["dir"];
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

	// Load optimization options
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

	// Interpret options
	if (optionsMap.Contains("interpret")) {
		Mango interpret = optionsMap["interpret"];

		if (interpret.Type() == MangoType::Boolean) {
			options.interpret = (bool)interpret;
		}
	}

	return options;
}

void MelonCompiler::Compile(const CompilerOptions& options) {
	// Setup
	CompilerOptions compOptions = SetupCompilerOptions(options);
	SymbolTable::Setup();
	LogMessage::LoadMessages("Melon/errors.mango");

	try {
		// Parse + Scan
		ParsingInfo   info     = ParseProject(compOptions);
		ScanInfoStack scanInfo = ScanProject(compOptions, info);

		// Compile + Optimize
		Ptr<Kiwi::KiwiProgram> program = CompileProject(info, scanInfo);
		//List<Instruction> optimizedInstructions = KiwiOptimizer::Optimize(instructions, compOptions.kiwiOptimizationPasses);

		// Output melon
		if (compOptions.outputMelon) {
			info.root.ToMelonFiles(compOptions);
		}

		// Output kiwi
		if (compOptions.outputKiwi) {
			OutputKiwi(compOptions, program);
		}

		// Output assembly
		if (compOptions.outputAssembly) {
			//OutputAssembly(compOptions, optimizedInstructions);
		}

		ErrorLog::Success(LogMessage("success.compile"), FileInfo());

		// Interpret kiwi
		if (compOptions.interpret) {
			Kiwi::Interpreter::InterpreterData data;
			data.program = program;
			program->Interpret(data);
		}
	}
	catch (CompileError& e) {
		ErrorLog::Fatal(LogMessage("fatal.compile"), FileInfo());
	}
}

CompilerOptions MelonCompiler::SetupCompilerOptions(const CompilerOptions& options) {
	CompilerOptions compOptions = options;
	compOptions.includeDirectories = List<String>();

	// Copy include directories
	for (const String& dir : options.includeDirectories) {
		compOptions.includeDirectories.Add(dir);
	}

	// Add main directory
	compOptions.includeDirectories.Add(Path::GetDirectory(options.mainFile));

	// Setup output file
	if (compOptions.outputName.Length() == 0) {
		compOptions.outputName = Path::GetFileName(options.mainFile);
	}

	// Setup output directory as main directory
	if (compOptions.outputDirectory.IsEmpty()) {
		compOptions.outputDirectory = compOptions.includeDirectories.Last();
	}

	// Cleanup output directory
	if (System::DirectoryExists(compOptions.outputDirectory)) {
		System::DeleteDirectory(compOptions.outputDirectory);
	}

	if (!System::DirectoryExists(compOptions.outputDirectory)) {
		System::CreateDirectory(compOptions.outputDirectory);
	}

	return compOptions;
}

ParsingInfo MelonCompiler::ParseProject(const CompilerOptions& options) {
	ParsingInfo info = Parser::Parse(options.mainFile, options);

	if (ErrorLog::HasError()) {
		throw CompileError();
	}
	
	Node::_root = &info.root;

	ErrorLog::AddMarker();
	info.root.IncludeScan(info);
	ErrorLog::Revert();

	info.root.parsingInfo = &info;

	return info;
}

ScanInfoStack MelonCompiler::ScanProject(const CompilerOptions& options, ParsingInfo& info) {
	ScanInfoStack scanInfo;

	for (UInt i = 0; i < (ULong)options.astOptimizationPasses + 1; i++) {
		if (i > 0) {
			OptimizeInfo opInfo;
			opInfo.usedVariables = scanInfo.usedVariables;
			opInfo.usedTypes     = scanInfo.usedTypes;
			opInfo.usedFunctions = scanInfo.usedFunctions;
			info.root.Optimize(opInfo);

			if (ErrorLog::HasError()) {
				throw CompileError();
			}

			if (!opInfo.optimized) break;

			scanInfo = ScanInfoStack();
		}

		scanInfo = info.root.Scan();

		if (ErrorLog::HasError()) {
			throw CompileError();
		}
	}

	return scanInfo;
}

List<OptimizerInstruction> MelonCompiler::CompileProjectOld(ParsingInfo& info, ScanInfoStack& scanInfo) {
	List<OptimizerInstruction> instructions = info.root.Compile(scanInfo.usedVariables);

	if (ErrorLog::HasError()) {
		throw CompileError();
	}

	return instructions;
}

Ptr<Kiwi::KiwiProgram> MelonCompiler::CompileProject(ParsingInfo& info, ScanInfoStack& scanInfo) {
	Ptr<Kiwi::KiwiProgram> program = new Kiwi::KiwiProgram();

	CompileInfo compileInfo;
	compileInfo.program = program;

	info.root.Compile(compileInfo);

	if (ErrorLog::HasError()) {
		throw CompileError();
	}

	return program;
}

void MelonCompiler::OutputKiwi(const CompilerOptions& options, const List<Instruction>& instructions) {
	KiwiLang::WriteToFile(Path::SetExtension(Path::Combine(options.outputDirectory, options.outputName), "kiwi"), ErrorLog::logger, instructions);

	for (const Tuple<Logger::LogLevel, String>& err : ErrorLog::logger) {
		if (err.value1 == Logger::LogLevel::Error) {
			throw CompileError();
		}
	}
}

void MelonCompiler::OutputKiwi(const CompilerOptions& options, Weak<Kiwi::KiwiProgram> program) {
	StringBuilder builder;
	program->BuildString(builder);

	FileWriter::WriteText(Path::SetExtension(Path::Combine(options.outputDirectory, options.outputName), "kiwi"), builder.ToString());
}

void MelonCompiler::OutputAssembly(const CompilerOptions& options, const List<Instruction>& instructions) {
	KiwiLang::WriteToFile(Path::SetExtension(Path::Combine(options.outputDirectory, options.outputName), "asm"), options.converter, instructions);

	for (const Tuple<Logger::LogLevel, String>& err : ErrorLog::logger) {
		if (err.value1 == Logger::LogLevel::Error) {
			throw CompileError();
		}
	}
}
