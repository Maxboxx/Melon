#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B RefNode
		/// Node for references
		class RefNode : public Node {
		public:

			///T Node
			/// The node to reference
			NodePtr node;

			RefNode(const NodePtr& node);
			~RefNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Boxx::List<Symbols::ScopeList> Types() const override;
			virtual Symbols::Symbol GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}