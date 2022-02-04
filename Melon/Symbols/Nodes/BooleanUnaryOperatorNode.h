#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unary operators on boolean values.
			class BooleanUnaryOperatorNode : public UnarySymbolNode {
			public:
				/// The unary operation.
				Kiwi::InstructionType op;

				/// Constructor.
				BooleanUnaryOperatorNode(const Kiwi::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}