#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B OptionalToBooleanNode
			/// Node for converting an optional type to bool
			class OptionalToBooleanNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The optional type
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}