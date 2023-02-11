#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Does nothing.
			class EmptySymbolNode : public ConstantSymbolNode {
			public:
				virtual Ptr<Kiwi::Value> Compile(Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}

