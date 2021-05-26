#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for statements.
		class StatementsNode final : public StatementNode {
		public:
			/// The list of statements.
			Boxx::List<StatementNode*> statements;

			StatementsNode(Symbols::Symbol* const scope, const FileInfo& file);
			~StatementsNode();

			virtual Boxx::UInt GetSize() const override;

			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual StatementNode* Optimize(OptimizeInfo& info) override;
			virtual CompiledNode Compile(CompileInfo& info) override;

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

		private:
			static bool HasSpaceAround(StatementNode* const node);
		};
	}
}