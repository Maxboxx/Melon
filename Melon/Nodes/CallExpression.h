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

			virtual Symbols::Symbol* Symbol() const override;
			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types(Symbols::TypeSymbol* expected = nullptr) const override;
		};
	}
}
