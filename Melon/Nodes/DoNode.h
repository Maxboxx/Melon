#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class DoNode : public Node {
		public:
			NodePtr nodes;

			DoNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~DoNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}