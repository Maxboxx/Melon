#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B IntegerToBoolNode
			/// Node for converting an integer to a boolean
			class IntegerToBoolNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The integer value to convert to a boolean
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}