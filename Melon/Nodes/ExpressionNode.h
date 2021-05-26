#pragma once

#include "Node.h"

#include "Melon/Symbols/TypeSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class ExpressionNode : public Node {
		public:
			/// Returns the type of the expression.
			virtual Symbols::TypeSymbol* Type() const;

			/// Returns the types of the expression.
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const;
		};
	}
}