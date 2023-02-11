#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		class Assignment;

		/// Node for conditional expressions.
		class Condition : public Node {
		public:
			/// The expression for a regular condition.
			Ptr<Expression> expression;

			/// The assignment for assign conditions.
			Ptr<Assignment> assign;

			Condition(Symbols::Symbol* const scope, const FileInfo& file);
			~Condition();

			/// Returns the type of the condition.
			virtual Symbols::TypeSymbol* Type() const;

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Condition> Optimize(OptimizeInfo& info);
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			/// Creates a condition from an expression.
			static Ptr<Condition> FromExpression(Ptr<Expression> expression);

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}