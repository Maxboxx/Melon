#pragma once

#include "Node.h"

#include "Melon/Symbols/TypeSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class Expression : public Node {
		public:
			Expression(Symbols::Symbol* const scope, const FileInfo& file);
			~Expression();

			/// Returns the type of the expression.
			virtual Symbols::TypeSymbol* Type() const;

			/// Returns the types of the expression.
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const;

			/// Returns the symbol of the expression or {nullptr} if the expression does not have a symbol.
			virtual Symbols::Symbol* Symbol() const {
				return nullptr;
			}

			/// Returns the specific symbol type of the expression.
			///p Returns {nullptr} if the symbol is not of the specified type.
			///M
			template <class T>
			T* Symbol() const {
			///M
				if (Symbols::Symbol* const sym = Symbol()) {
					return sym->Cast<T>();
				}

				return nullptr;
			}

			/// Optimizes the expression.
			///R A new optimized node or {nullptr} if the node was not optimized.
			virtual Ptr<Expression> Optimize(OptimizeInfo& info);

			/// True if the expression is an immediate Kiwi value.
			virtual bool IsImmediate() const;

			/// Returns the immediate value of the expression.
			virtual Boxx::Long GetImmediate() const;
		};
	}
}