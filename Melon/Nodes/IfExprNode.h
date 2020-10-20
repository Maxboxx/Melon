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

			IfExprNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~IfExprNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}