#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ForConditionNode
		/// Node for loop structures
		class ForConditionNode : public Node {
		public:
			NodePtr loopInit;

			Boxx::Optional<Symbols::Scope> conditionOperator;
			NodePtr loopCondition;

			Boxx::Optional<Symbols::Scope> stepOperator;
			NodePtr loopStep;

			ForConditionNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ForConditionNode();

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}