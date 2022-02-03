#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unwrapping optional types.
			class OptionalUnwrapNode : public UnarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::Expression& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}