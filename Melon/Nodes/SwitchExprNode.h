#pragma once

#include "ExpressionNode.h"
#include "SwitchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} expressions.
		class SwitchExprNode : public SwitchBaseNode<Expression> {
		public:
			SwitchExprNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchExprNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual _Expression_ Optimize(OptimizeInfo& info) override;
		};
	}
}
