#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B BooleanNotNode
			class BooleanNotNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The boolean value to perform the operatoion on
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}