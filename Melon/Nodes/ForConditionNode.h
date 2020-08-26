#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ForConditionNode
		/// Node for loop structures
		class ForConditionNode : public Node {
		public:
			NodePtr loopInit;
			NodePtr loopCondition;
			NodePtr loopStep;

			ForConditionNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ForConditionNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}