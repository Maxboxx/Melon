#pragma once

#include "BinaryOperatorExpression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for the default operator.
		class DefaultExpression : public BinaryOperatorExpression {
		public:
			DefaultExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~DefaultExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}