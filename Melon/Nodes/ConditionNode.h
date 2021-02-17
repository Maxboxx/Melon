#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class AssignNode;

		///B ConditionNode
		/// Node for conditional expressions
		class ConditionNode : public Node {
		public:

			///T Condition
			NodePtr cond;

			ConditionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ConditionNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

			CompiledNode CompileAssignCondition(Boxx::Pointer<AssignNode>& assign, CompileInfo& info);
		};
	}
}