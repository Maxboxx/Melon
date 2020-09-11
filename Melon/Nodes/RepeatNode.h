#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class RepeatNode : public Node {
		public:
			NodePtr condition;
			NodePtr content;

			RepeatNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~RepeatNode();

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}