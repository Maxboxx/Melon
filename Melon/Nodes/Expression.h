#pragma once

#include "Node.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class Expression : public Node {
		public:
			Expression(Symbols::Symbol* const scope, const FileInfo& file);

			/// Returns the type of the expression.
			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const;

			/// Returns the types of the expression.
			virtual Boxx::List<Symbols::TypeSymbol*> Types(Symbols::TypeSymbol* expected = nullptr) const;

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

			/// Gets the assign function for the expression.
			virtual Symbols::FunctionSymbol* AssignFunc(Symbols::TypeSymbol* type) const;

			/// Compiles the assign function.
			virtual Ptr<Kiwi::Value> CompileAssignFunc(Symbols::FunctionSymbol* func, Weak<Expression> expr, CompileInfo& info) const;
		};

		/// A weak expression.
		class WeakExpression : public Expression {
		public:
			/// The weak expression.
			Weak<Expression> expression;

			WeakExpression(Weak<Expression> expr) : Expression(expr->scope, expr->File()) {expression = expr;}
			~WeakExpression() {}

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override {return expression->Type(expected);}
			virtual Boxx::List<Symbols::TypeSymbol*> Types(Symbols::TypeSymbol* expected = nullptr) const override {return expression->Types(expected);}
			virtual Symbols::Symbol* Symbol() const override {return expression->Symbol();}
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override {return nullptr;}
			virtual void IncludeScan(Parsing::ParsingInfo& info) override {expression->IncludeScan(info);}
			virtual ScanResult Scan(ScanInfoStack& info) override {return expression->Scan(info);}
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override {return expression->Compile(info);}
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override {return expression->ToMelon(indent);}
		};
	}
}
