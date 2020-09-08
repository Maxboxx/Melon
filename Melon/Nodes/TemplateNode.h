#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class TemplateNode : public Node {
		public:
			NodePtr node;

			Boxx::List<Symbols::ScopeList> templateArgs;

			/*TemplateNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~TemplateNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;*/
		};
	}
}