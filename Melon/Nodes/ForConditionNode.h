#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the conditions of for loops.
		class ForConditionNode : public Node {
		public:
			/// The init of the for condition.
			NodePtr loopInit;

			/// An optional condition operator.
			///p The condition is automatically compared to the loop variable if this is set.
			Boxx::Optional<Symbols::Scope> conditionOperator;

			/// The loop condition.
			NodePtr loopCondition;

			/// An optional step operator.
			///p The loop variable is automatically updated if this is set.
			Boxx::Optional<Symbols::Scope> stepOperator;

			/// The loop step.
			NodePtr loopStep;

			ForConditionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ForConditionNode();

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}