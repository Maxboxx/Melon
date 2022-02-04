#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {if} expressions.
		class IfExprNode : public Expression {
		public:
			/// The nodes for each segment of the {if} expression.
			Boxx::List<_Expression_> nodes;

			/// The conditions for the segments.
			Boxx::List<_Condition_> conditions;

			IfExprNode(Symbols::Symbol* const scope, const FileInfo& file);
			~IfExprNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual _Expression_ Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}