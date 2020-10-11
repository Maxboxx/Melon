#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B OptionalNotNode
			class OptionalNotNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The optional typ to perform the operation on
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}