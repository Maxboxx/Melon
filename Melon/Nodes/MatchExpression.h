#pragma once

#include "Expression.h"
#include "MatchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} expressions.
		class MatchExpression : public MatchBaseNode<Expression> {
		public:
			MatchExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~MatchExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
		};
	}
}
