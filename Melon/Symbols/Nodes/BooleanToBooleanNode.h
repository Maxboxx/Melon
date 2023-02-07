#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for converting a boolean to a boolean. It's extremely useful.
			class [[deprecated]] BooleanToBooleanNode : public UnarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}

