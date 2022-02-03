#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for boolean not on integer.
			class IntegerNotNode : public UnarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::Expression& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}