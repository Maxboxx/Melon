#pragma once

#include "IntegerBaseNode.h"

#include "Kiwi/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unary operators on integers.
			class IntegerUnaryOperatorNode : public IntegerBaseNode<UnarySymbolNode> {
			public:
				/// The unary operation.
				Kiwi::InstructionType op;

				/// Constructor.
				IntegerUnaryOperatorNode(const Boxx::UByte size, const Kiwi::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}