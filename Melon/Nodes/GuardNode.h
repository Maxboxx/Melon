#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class GuardNode : public Node {
		public:

			///T Match expression
			NodePtr cond;

			///T Else
			/// All statements in the else block
			NodePtr else_;

			///T Continue
			/// All statements after the guard
			NodePtr continue_;

			///T End
			NodePtr end;

			GuardNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~GuardNode();

			void AddScopeBreak(ScanInfoStack& info);

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}