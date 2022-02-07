#pragma once

#include "Expression.h"
#include "SwitchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} expressions.
		class SwitchExpression : public SwitchBaseNode<Expression> {
		public:
			SwitchExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
		};
	}
}
