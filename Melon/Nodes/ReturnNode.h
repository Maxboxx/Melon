#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ReturnNode
		/// Node for function returns
		class ReturnNode : public Node {
		public:

			///T Nodes
			/// The nodes to return from the function
			Boxx::List<NodePtr> nodes;

			///T Function
			/// The absolute name of the function to return from
			Symbols::ScopeList func;

			ReturnNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ReturnNode();

			///T Get Function
			/// Get the function for the return
			Symbols::FunctionSymbol* GetFunc() const;

			///T Get Types
			/// Get the symbols of the return value types
			Boxx::List<Symbols::TypeSymbol*> GetTypes() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}