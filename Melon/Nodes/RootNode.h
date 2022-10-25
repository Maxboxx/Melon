#pragma once

#include "Node.h"
#include "Statements.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"

namespace Melon {
	struct CompilerOptions;

	///N Melon::Nodes
	namespace Nodes {
		class FunctionBody;

		/// The root node of the compiled program.
		class RootNode final : public Node {
		public:
			/// All statements in the main scope of the program.
			Boxx::List<Ptr<Statements>> nodes;

			/// All functions in the program.
			Boxx::List<Ptr<FunctionBody>> funcs;

			/// The id for the next function.
			Boxx::UInt funcId = 0;

			RootNode();
			~RootNode();

			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;

			/// Returns the instruction list from {Node::Compile()}.
			Boxx::List<Optimizing::OptimizerInstruction> Compile(const Boxx::Set<Symbols::VariableSymbol*>& usedVariables);

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			void Optimize(OptimizeInfo& info);

			/// Adds a template specialization.
			void AddTemplateSpecialization(const Symbols::NameList& name, const Symbols::NameList& scope, const FileInfo& file, const bool scan = true);

			/// Scans for errors.
			ScanInfoStack Scan();

			/// Writes the optimized code to melon files.
			void ToMelonFiles(const CompilerOptions& options) const;

			/// The current parsing info.
			Parsing::ParsingInfo* parsingInfo = nullptr;

		private:
			Boxx::Tuple<Symbols::TemplateTypeSymbol*, Boxx::List<Symbols::NameList>> FindTemplateArgs(const Symbols::NameList& name, const Symbols::NameList& scope, const FileInfo& file);

			Boxx::UInt nodeIndex = 0;
			Boxx::UInt funcIndex = 0;
			Boxx::UInt templateIndex = 0;

			bool includeScanning = false;
		};
	}
}