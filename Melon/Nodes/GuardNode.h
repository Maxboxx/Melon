#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {guard} statements.
		class GuardNode : public Node {
		public:
			/// The guard condition.
			NodePtr cond;

			/// The content of the {else} block.
			NodePtr else_;

			/// The statements after the {guard} statement.
			NodePtr continue_;

			/// The final break of the {else} block.
			NodePtr end;

			GuardNode(Symbols::Symbol* const scope, const FileInfo& file);
			~GuardNode();
			
			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

			void AddScopeBreak(ScanInfoStack& info);

			void CompileElse(CompiledNode& compiled, CompileInfo& info, Boxx::List<Boxx::UInt>& jumps);

			NodePtr OptimizeFalseCondition(OptimizeInfo& info);
			NodePtr OptimizeTrueCondition(OptimizeInfo& info);

			void AddScopeWiseBreak(ScanInfoStack& info);
			void AddContinue(ScanInfoStack& info);
			void AddReturn(ScanInfoStack& info);
			void AddThrow(ScanInfoStack& info);
		};
	}
}