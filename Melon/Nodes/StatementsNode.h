#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B StatementsNode
		/// Node for statements
		class StatementsNode : public Node {
		public:

			///T List of statements
			Boxx::List<NodePtr> statements;

			StatementsNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~StatementsNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}