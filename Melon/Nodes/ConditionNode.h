#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class ConditionNode : public Node {
		public:

			///T condition
			NodePtr cond;

			ConditionNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ConditionNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String conditionInstName;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}