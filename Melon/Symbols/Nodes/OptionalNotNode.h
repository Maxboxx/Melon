#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for {not} operation on an optional value.
			class OptionalNotNode : public UnarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}