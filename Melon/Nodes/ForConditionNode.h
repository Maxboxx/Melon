#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		class AssignNode;

		/// Node for the conditions of for loops.
		class ForConditionNode : public Node {
		public:
			/// The init of the for condition.
			Pointer<Node> loopInit;

			/// An optional condition operator.
			///p The condition is automatically compared to the loop variable if this is set.
			Boxx::Optional<Symbols::Name> conditionOperator;

			/// The loop condition.
			Condition loopCondition;

			/// An optional step operator.
			///p The loop variable is automatically updated if this is set.
			Boxx::Optional<Symbols::Name> stepOperator;

			/// The loop step.
			Pointer<Node> loopStep;

			ForConditionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ForConditionNode();

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Pointer<ForConditionNode> Optimize(OptimizeInfo& info);
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}