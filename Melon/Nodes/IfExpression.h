#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {if} expressions.
		class IfExpression : public Expression {
		public:
			/// The nodes for each segment of the {if} expression.
			Boxx::List<Ptr<Expression>> nodes;

			/// The conditions for the segments.
			Boxx::List<Ptr<Condition>> conditions;

			IfExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~IfExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}