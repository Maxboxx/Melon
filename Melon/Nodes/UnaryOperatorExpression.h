#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for unary operators.
		class UnaryOperatorExpression : public Expression {
		public:
			/// The operand.
			Ptr<Expression> operand;

			/// The operator to use.
			Symbols::Name op;

			UnaryOperatorExpression(Symbols::Symbol* const scope, const Symbols::Name& op, const FileInfo& file);
			~UnaryOperatorExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* Symbol() const override;

			/// Gets the operator.
			virtual Symbols::Name GetOperator() const;

			virtual CompiledNode Compile(OldCompileInfo& info) override;
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