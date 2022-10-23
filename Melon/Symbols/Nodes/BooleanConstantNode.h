#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for a boolean constant.
			class BooleanConstantNode : public ConstantSymbolNode {
			public:
				/// The boolean value.
				bool value;

				/// Constructor for a boolean value.
				BooleanConstantNode(const bool value);

				virtual Melon::Nodes::CompiledNode Compile(Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}