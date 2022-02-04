#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for assigning structs.
			class StructAssignNode : public BinarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand1, const Melon::Nodes::_Expression_& operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}