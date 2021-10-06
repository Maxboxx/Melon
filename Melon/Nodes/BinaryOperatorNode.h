#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for binary operators.
		class BinaryOperatorNode : public ExpressionNode {
		public:
			/// The operands.
			Expression operand1, operand2;

			/// The operator.
			Symbols::Name op;

			/// Creates a binary operator node.
			BinaryOperatorNode(Symbols::Symbol* const scope, const Symbols::Name& op, const FileInfo& file);
			~BinaryOperatorNode();

			virtual Symbols::TypeSymbol* Type() const override;

			/// Gets the operator.
			virtual Symbols::Name GetOperator() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Expression Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}