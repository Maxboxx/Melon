#pragma once

#include "Node.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"

namespace Melon {
	struct CompilerOptions;

	///N Melon::Nodes
	namespace Nodes {
		/// The root node of the compiled program.
		class RootNode : public Node {
		public:
			/// All nodes in the main scope of the program.
			Boxx::List<NodePtr> nodes;

			/// All functions in the program.
			Boxx::List<NodePtr> funcs;

			/// The id for the next function.
			Boxx::UInt funcId = 0;

			RootNode();
			~RootNode();

			virtual CompiledNode Compile(CompileInfo& info) override;

			/// Returns the instruction list from {Node::Compile()}.
			Boxx::List<Optimizing::OptimizerInstruction> Compile(const Boxx::Set<Symbols::VariableSymbol*>& usedVariables);

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;

			/// Adds a template specialization.
			void AddTemplateSpecialization(const Symbols::ScopeList& name, const Symbols::ScopeList& scope, const FileInfo& file, const bool scan = true);

			/// Scans for errors.
			ScanInfoStack Scan();

			/// Writes the optimized code to melon files.
			void ToMelonFiles(const CompilerOptions& options) const;

			/// The current parsing info.
			Parsing::ParsingInfo* parsingInfo = nullptr;

		private:
			Boxx::Tuple<Symbols::TemplateTypeSymbol*, Boxx::List<Symbols::ScopeList>> FindTemplateArgs(const Symbols::ScopeList& name, const Symbols::ScopeList& scope, const FileInfo& file);

			Boxx::UInt nodeIndex = 0;
			Boxx::UInt funcIndex = 0;
			Boxx::UInt templateIndex = 0;

			bool includeScanning = false;
		};
	}
}