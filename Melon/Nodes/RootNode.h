#pragma once

#include "Node.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"

namespace Melon {
	struct CompilerOptions;

	namespace Nodes {

		///B RootNode
		/// The root node of the compiled program
		class RootNode : public Node {
		public:

			///H Members

			///T Nodes
			/// All nodes in the main scope of the program
			Boxx::List<NodePtr> nodes;

			///T Functions
			/// All functions in the program
			Boxx::List<NodePtr> funcs;

			///T Function id
			/// The id for the next function
			Boxx::UInt funcId = 0;

			///H Constructors

			///T Empty
			RootNode();
			~RootNode();

			///H Methods

			virtual CompiledNode Compile(CompileInfo& info) override;

			///T Compile
			/// Returns the instruction list from <code>Node::Compile()</code>
			Boxx::List<Optimizing::OptimizerInstruction> Compile(const Boxx::Set<Symbols::ScopeList>& usedVariables);

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;

			///T Add Template Specialization
			void AddTemplateSpecialization(const Symbols::ScopeList& name, const Symbols::ScopeList& scope, const FileInfo& file, const bool scan = true);

			///T Scan
			/// Used for scanning for errors
			ScanInfoStack Scan();

			///T ToString
			/// Converts the result from <code>ToMango()</code> to a string
			Boxx::String ToString() const;

			///T To Melon Files
			/// Writes the optimized code to melon files
			void ToMelonFiles(const CompilerOptions& options) const;

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