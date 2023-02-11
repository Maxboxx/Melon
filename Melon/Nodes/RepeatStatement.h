#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {reapeat} statements.
		class RepeatStatement : public Statement {
		public:
			/// The {until} condition.
			Ptr<Condition> condition;

			/// The loop body.
			Ptr<Statements> statements;

			RepeatStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~RepeatStatement();

			virtual bool IsScope() const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}