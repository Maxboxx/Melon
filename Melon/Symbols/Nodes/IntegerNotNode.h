#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B IntegerNotNode
			class IntegerNotNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The integer value to perform the operatoion on
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}