#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B OptionalUnwrapNode
			/// Node for assigning values to optional types
			class OptionalUnwrapNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The optional to unwrap
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}