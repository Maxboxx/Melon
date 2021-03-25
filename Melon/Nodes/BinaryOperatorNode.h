#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for binary operators.
		class BinaryOperatorNode : public Node {
		public:
			/// The operands.
			NodePtr node1, node2;

			/// The operator.
			Symbols::Scope op;

			/// Creates a binary operator node.
			BinaryOperatorNode(Symbols::Symbol* const scope, const Symbols::Scope& op, const FileInfo& file);
			~BinaryOperatorNode();

			virtual Symbols::TypeSymbol* Type() const override;

			/// Gets the operator.
			virtual Symbols::Scope GetOperator() const;

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