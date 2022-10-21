#pragma once

#include "Boxx/String.h"
#include "Boxx/List.h"
#include "Boxx/Pointer.h"

#include "Kiwi/Old/Kiwi.h"
#include "Kiwi/Old/x86_64Converter.h"

namespace Melon {
	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Nodes {
		struct ScanInfoStack;
	}

	namespace Optimizing {
		struct OptimizerInstruction;
	}

	///B CompilerOptions
	/// Options for the compilation process
	struct CompilerOptions {
		///H Values

		///T Include Directories
		/// Contains the absolute file path for all include directories
		/// The directories should end with a <code>/</code>
		/// The directory the main melon file is located in is always added to the include directories by the compiler
		Boxx::List<Boxx::String> includeDirectories;

		///T AST Optimization Passes
		/// The amount of times to perform optimizations on the AST
		Boxx::UInt astOptimizationPasses = 0;

		///T Kiwi Optimization Passes
		/// The amount of times to perform optimizations on the kiwi instructions
		Boxx::UInt kiwiOptimizationPasses = 0;

		///T Main file
		/// The main melon file for the project
		Boxx::String mainFile;

		///T Output Directory
		/// The directory of the output files
		/// The directory should end with a <code>/</code>
		/// Defaults to the directory of the main melon file
		Boxx::String outputDirectory;

		///T Output Name
		/// The name of the output files
		/// Defaults to the name of the main melon file
		Boxx::String outputName;

		///T Output files
		/// Boolean values for choosing which files the compiler should output
		///M
		bool outputMelon      = false;
		bool outputKiwi       = false;
		bool outputAssembly   = false;
		bool outputExecutable = true;
		///M

		///T Create Error Log
		bool createErrorLog = true;

		///T Converter
		/// The kiwi converter to use for compiling
		Boxx::Pointer<KiwiOld::Converter> converter = new KiwiOld::x86_64Converter(KiwiOld::x86_64Syntax::Intel);

		~CompilerOptions() {}

		///H Static functions
		
		///T Load from Mango
		/// Loads the compiler options from a mango file
		static CompilerOptions LoadFromFile(const Boxx::String& mangoFile);
	};

	///B MelonCompiler
	/// The main class for compiling melon files
	class MelonCompiler {
	public:
		///T Compile
		/// Compiles a melon project
		///A mainMelonFile: The main melon file for the project
		///A options: The compiler options to use for the compilation
		static void Compile(const CompilerOptions& options);

	private:
		static CompilerOptions SetupCompilerOptions(const CompilerOptions& options);

		static Parsing::ParsingInfo ParseProject(const CompilerOptions& options);
		static Nodes::ScanInfoStack ScanProject(const CompilerOptions& options, Parsing::ParsingInfo& info);
		static Boxx::List<Optimizing::OptimizerInstruction> CompileProject(Parsing::ParsingInfo& info, Nodes::ScanInfoStack& scanInfo);

		static void OutputKiwi(const CompilerOptions& options, const Boxx::List<KiwiOld::Instruction>& instructions);
		static void OutputAssembly(const CompilerOptions& options, const Boxx::List<KiwiOld::Instruction>& instructions);
	};
}
