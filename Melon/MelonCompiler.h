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

	/// Options for the compilation process
	struct CompilerOptions {
		/// Contains the absolute file path for all include directories.
		/// The directories should end with a {/}.
		/// The directory the main melon file is located in is always added to the include directories by the compiler.
		Boxx::List<Boxx::String> includeDirectories;

		/// The amount of times to perform optimizations on the AST.
		Boxx::UInt astOptimizationPasses = 0;

		/// The amount of times to perform optimizations on the kiwi instructions.
		Boxx::UInt kiwiOptimizationPasses = 0;

		/// The main melon file for the project.
		Boxx::String mainFile;

		/// The directory of the output files.
		/// The directory should end with a <code>/</code>.
		/// Defaults to the directory of the main melon file.
		Boxx::String outputDirectory;

		/// The name of the output files.
		/// Defaults to the name of the main melon file.
		Boxx::String outputName;

		/// {true} if the compiler should output melon code.
		bool outputMelon      = false;

		/// {true} if the compiler should output kiwi code. 
		bool outputKiwi       = false;

		/// {true} if the compiler should output assembly code.
		bool outputAssembly   = false;

		/// {true} if the compiler should output an executable file.
		bool outputExecutable = true;

		/// {true} if an error log file should be created.
		bool createErrorLog = true;

		/// The kiwi converter to use for compiling.
		Boxx::Pointer<KiwiOld::Converter> converter = new KiwiOld::x86_64Converter(KiwiOld::x86_64Syntax::Intel);

		~CompilerOptions() {}
		
		/// Loads the compiler options from a mango file.
		static CompilerOptions LoadFromFile(const Boxx::String& mangoFile);
	};

	/// The main class for compiling melon files.
	class MelonCompiler {
	public:
		/// Compiles a melon project.
		///A mainMelonFile: The main melon file for the project.
		///A options: The compiler options to use for the compilation.
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
