#pragma once

#include "Node.h"

#include "Melon/Symbols/TypeSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class ExpressionNode : public Node {
		public:
			ExpressionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ExpressionNode();

			/// Returns the type of the expression.
			virtual Symbols::TypeSymbol* Type() const;

			/// Returns the types of the expression.
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const;

			/// Returns the symbol of the expression or {nullptr} if the expression does not have a symbol.
			virtual Symbol* Symbol() const;

			/// Returns the specific symbol type of the expression.
			///p Returns {nullptr} if the symbol is not of the specified type.
			///M
			template <class T>
			T* Symbol() const {
			///M
				if (Symbol* const sym = Symbol()) {
					return sym->Cast<T>();
				}

				return nullptr;
			}

			/// Optimizes the expression.
			///R A new optimized node or {nullptr} if the node was not optimized.
			virtual ExpressionNode* Optimize(OptimizeInfo& info);
		};
	}
}