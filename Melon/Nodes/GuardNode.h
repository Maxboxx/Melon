#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class GuardNode : public Node {
		public:

			///T Match expression
			NodePtr cond;

			///T Else
			NodePtr else_;

			GuardNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~GuardNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}