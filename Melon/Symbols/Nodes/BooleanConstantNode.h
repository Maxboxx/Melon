#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B BooleanConstantNode
			class BooleanConstantNode : public SymbolNode {
			public:
				bool value;

				BooleanConstantNode(const bool value);

				///T Compile
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}