#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h";

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between boolean values.
			class BooleanBinaryOperatorNode : public BinarySymbolNode {
			public:
				/// The operator.
				Kiwi::InstructionType op;

				/// Constructor with operator.
				BooleanBinaryOperatorNode(const Kiwi::InstructionType op);

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand1, const Melon::Nodes::_Expression_& operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}