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

			/// {true} if the expression is an immediate Kiwi value.
			virtual bool IsImmediate() const;

			/// Returns the immediate value of the expression.
			virtual Boxx::Long GetImmediate() const;
		};

		/// A weak expression.
		class WeakExpression : public Expression {
		public:
			/// The weak expression.
			Weak<Expression> expression;

			WeakExpression(Weak<Expression> expr) : Expression(expr->scope, expr->File()) {expression = expr;}
			~WeakExpression() {}

			virtual Symbols::TypeSymbol* Type() const override {return expression->Type();}
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override {return expression->Types();}
			virtual Symbols::Symbol* Symbol() const override {return expression->Symbol();}
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override {return nullptr;}
			virtual bool IsImmediate() const override {return expression->IsImmediate();}
			virtual Boxx::Long GetImmediate() const override {return expression->GetImmediate();}
			virtual void IncludeScan(Parsing::ParsingInfo& info) override {expression->IncludeScan(info);}
			virtual ScanResult Scan(ScanInfoStack& info) override {return expression->Scan(info);}
			virtual CompiledNode Compile(CompileInfo& info) override {return expression->Compile(info);}
			virtual Boxx::UInt GetSize() const override {return expression->GetSize();}
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override {return expression->ToMelon(indent);}
		};
	}
}
