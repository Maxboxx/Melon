#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		class AssignNode;

		/// Node for conditional expressions.
		class ConditionNode : public Node {
		public:
			/// The expression for a regular condition.
			Expression cond;

			/// The assign node for assign conditions.
			Boxx::Pointer<AssignNode> assign;

			ConditionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ConditionNode();

			/// True if the expression is an immediate Kiwi value.
			virtual bool IsImmediate() const;

			/// Returns the immediate value of the expression.
			virtual Boxx::Long GetImmediate() const;

			/// Returns the type of the condition.
			virtual Symbols::TypeSymbol* Type() const;

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Condition Optimize(OptimizeInfo& info);
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			CompiledNode CompileAssignCondition(CompileInfo& info);
		};
	}
}