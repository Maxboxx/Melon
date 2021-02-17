#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		///B GuardNode
		/// Node for guard statements
		class GuardNode : public Node {
		public:

			///T Condition
			NodePtr cond;

			///T Else
			/// All statements in the else block
			NodePtr else_;

			///T Continue
			/// All statements after the guard
			NodePtr continue_;

			///T End
			/// The final break of the else
			NodePtr end;

			GuardNode(Symbols::Symbol* const scope, const FileInfo& file);
			~GuardNode();

			///T Add Scope Break
			/// Adds a break to the end of the else scope
			void AddScopeBreak(ScanInfoStack& info);

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

			void CompileElse(CompiledNode& compiled, CompileInfo& info, Boxx::List<Boxx::UInt>& jumps);
		};
	}
}