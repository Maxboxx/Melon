#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ForConditionNode
		/// Node for the conditions of for loops
		class ForConditionNode : public Node {
		public:
			///T Loop Init
			NodePtr loopInit;

			///T Condition operator
			Boxx::Optional<Symbols::Scope> conditionOperator;

			///T Loop Condition
			NodePtr loopCondition;

			///T Step Operator
			Boxx::Optional<Symbols::Scope> stepOperator;

			///T Loop Step
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