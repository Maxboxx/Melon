#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for unary operators.
		class UnaryOperatorNode : public Expression {
		public:
			/// The operand.
			_Expression_ operand;

			/// The operator to use.
			Symbols::Name op;

			UnaryOperatorNode(Symbols::Symbol* const scope, const Symbols::Name& op, const FileInfo& file);
			~UnaryOperatorNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* Symbol() const override;

			/// Gets the operator.
			virtual Symbols::Name GetOperator() const;

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