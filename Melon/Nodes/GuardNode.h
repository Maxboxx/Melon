#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {guard} statements.
		class GuardNode : public Statement {
		public:
			/// The guard condition.
			_Condition_ cond;

			/// The content of the {else} block.
			_Statements_ else_;

			/// The statements after the {guard} statement.
			_Statements_ continue_;

			/// The final break of the {else} block.
			_Statement_ end;

			GuardNode(Symbols::Symbol* const scope, const FileInfo& file);
			~GuardNode();
			
			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual _Statement_ Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			void AddScopeBreak(ScanInfoStack& info);

			void CompileElse(CompiledNode& compiled, CompileInfo& info, Boxx::List<Boxx::UInt>& jumps);

			_Statement_ OptimizeFalseCondition(OptimizeInfo& info);
			_Statement_ OptimizeTrueCondition(OptimizeInfo& info);

			void AddScopeWiseBreak(ScanInfoStack& info);
			void AddContinue(ScanInfoStack& info);
			void AddReturn(ScanInfoStack& info);
			void AddThrow(ScanInfoStack& info);
		};
	}
}