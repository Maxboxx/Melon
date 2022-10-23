#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Does nothing.
			class EmptySymbolNode : public ConstantSymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}

