#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ReturnNode
		/// Node for function returns
		class ReturnNode : public Node {
		public:

			///T Nodes
			/// The nodes to return from the function
			Boxx::List<NodePtr> nodes;

			///T Function
			/// The absolute name of the function to return from
			Symbols::ScopeList func;

			ReturnNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ReturnNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}