#pragma once

#include "Node.h"

namespace Melon {
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
			Boxx::List<Optimizing::OptimizerInstruction> Compile();

			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;

			///T Scan
			/// Used for scanning for errors
			ScanInfoStack Scan();

			///T ToString
			/// Converts the result from <code>ToMango()</code> to a string
			Boxx::String ToString() const;
		};
	}
}