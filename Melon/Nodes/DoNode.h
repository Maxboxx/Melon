#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node for {do} statements.
		class DoNode : public Statement {
		public:
			/// The content of the do statement.
			_Statements_ statements;

			DoNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DoNode();

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