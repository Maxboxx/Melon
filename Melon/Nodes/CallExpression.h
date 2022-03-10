#pragma once

#include "Expression.h"
#include "BaseCallNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Call expression.
		class CallExpression : public BaseCallNode<Expression> {
		public:
			CallExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~CallExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
		};
	}
}
