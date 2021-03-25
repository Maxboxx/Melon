#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		class AssignNode;

		/// Node for conditional expressions.
		class ConditionNode : public Node {
		public:
			/// The condition.
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
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			CompiledNode CompileAssignCondition(Boxx::Pointer<AssignNode>& assign, CompileInfo& info);
		};
	}
}