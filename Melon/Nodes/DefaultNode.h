#pragma once

#include "Node.h"
#include "BinaryOperatorNode.h"

namespace Melon {
	namespace Nodes {
		class IfExprNode;

		///B DefaultNode
		/// Node for the default operator
		class DefaultNode : public BinaryOperatorNode {
		public:
			DefaultNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DefaultNode();

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