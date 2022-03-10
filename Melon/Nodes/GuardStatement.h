#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {guard} statements.
		class GuardStatement : public Statement {
		public:
			/// The guard condition.
			Ptr<Condition> cond;

			/// The content of the {else} block.
			Ptr<Statements> else_;

			/// The statements after the {guard} statement.
			Ptr<Statements> continue_;

			/// The final break of the {else} block.
			Ptr<Statement> end;

			GuardStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~GuardStatement();
			
			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			void AddScopeBreak(ScanInfoStack& info);

			void CompileElse(CompiledNode& compiled, CompileInfo& info, Boxx::List<Boxx::UInt>& jumps);

			Ptr<Statement> OptimizeFalseCondition(OptimizeInfo& info);
			Ptr<Statement> OptimizeTrueCondition(OptimizeInfo& info);

			void AddScopeWiseBreak(ScanInfoStack& info);
			void AddContinue(ScanInfoStack& info);
			void AddReturn(ScanInfoStack& info);
			void AddThrow(ScanInfoStack& info);
		};
	}
}