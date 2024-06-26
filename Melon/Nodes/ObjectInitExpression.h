#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for object init.
		class ObjectInitExpression : public Expression {
		public:
			/// The expression to perform object init on.
			Ptr<Expression> expression;

			/// The variables to assign values to.
			Boxx::List<Symbols::Name> vars;

			/// The expressions to assign to the variables.
			Boxx::List<Ptr<Expression>> expressions;

			ObjectInitExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~ObjectInitExpression();

			Symbols::TypeSymbol* ExprType(Symbols::TypeSymbol* expected = nullptr) const;
			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}