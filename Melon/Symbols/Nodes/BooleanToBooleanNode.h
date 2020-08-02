#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B BooleanToBooleanNode
			/// Node for converting a boolean to a boolean. It's extremely useful
			class BooleanToBooleanNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The boolean value to convert to a boolean
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}