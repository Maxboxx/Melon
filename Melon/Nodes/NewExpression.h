#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for unary operators.
		class NewExpression : public Expression {
		public:
			/// The operand.
			Ptr<Expression> operand;

			NewExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~NewExpression();

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Symbols::Symbol* Symbol() const override;

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			Symbols::TypeSymbol* ExpectedInnerType(Symbols::TypeSymbol* expectedOuter) const;
		};
	}
}