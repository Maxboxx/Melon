#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expresion for binary operators.
		class BinaryOperatorExpression : public Expression {
		public:
			/// The operands.
			Ptr<Expression> operand1, operand2;

			/// The operator.
			Symbols::Name op;

			/// Creates a binary operator node.
			BinaryOperatorExpression(Symbols::Symbol* const scope, const Symbols::Name& op, const FileInfo& file);
			~BinaryOperatorExpression();

			virtual Symbols::TypeSymbol* Type() const override;

			/// Gets the operator.
			virtual Symbols::Name GetOperator() const;

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