#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {return} statements.
		class ReturnNode : public Node {
		public:
			/// The nodes to return from the function.
			Boxx::List<NodePtr> nodes;

			/// The absolute name of the function to return from.
			Symbols::ScopeList func;

			ReturnNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ReturnNode();

			/// Get the function to return from.
			Symbols::FunctionSymbol* GetFunc() const;

			/// Get the symbols of the return value types.
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