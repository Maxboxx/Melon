#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for converting an integer to a boolean.
			class IntegerToBoolNode : public SymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::Expression& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}

