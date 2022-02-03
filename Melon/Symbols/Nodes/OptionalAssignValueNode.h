#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for assigning values to optional types.
			class OptionalAssignValueNode : public BinarySymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::Expression& operand1, const Melon::Nodes::Expression& operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}