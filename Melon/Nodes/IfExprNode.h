#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B IfExprNode
		/// Node for if-expressions
		class IfExprNode : public Node {
		public:

			///T Nodes
			/// The nodes for each segment of the if-expression
			Boxx::List<NodePtr> nodes;

			///T Conditions
			/// The conditions for the segments
			Boxx::List<NodePtr> conditions;

			IfExprNode(Symbols::Symbol* const scope, const FileInfo& file);
			~IfExprNode();

			virtual Symbols::TypeSymbol* Type() const override;
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