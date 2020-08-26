#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class ConvertNode : public Node {
		public:
			NodePtr node;
			Symbols::ScopeList type;
			bool isExplicit = false;

			ConvertNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ConvertNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}