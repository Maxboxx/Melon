#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B BinaryOperatorNode
		/// Node for binary operators
		class BinaryOperatorNode : public Node {
		public:

			///H Members

			///T Operands
			NodePtr node1, node2;

			///T Operator
			Symbols::Scope op;

			///H Constructors

			///T Constructor
			BinaryOperatorNode(Symbols::Symbol* const scope, const Symbols::Scope& op, const FileInfo& file);
			~BinaryOperatorNode();

			///H Methods

			virtual Symbols::TypeSymbol* Type() const override;

			///T Get operator
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