#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {reapeat} statements.
		class RepeatNode : public Statement {
		public:
			/// The {until} condition.
			_Condition_ condition;

			/// The loop body.
			_Statements_ statements;

			RepeatNode(Symbols::Symbol* const scope, const FileInfo& file);
			~RepeatNode();

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual _Statement_ Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}