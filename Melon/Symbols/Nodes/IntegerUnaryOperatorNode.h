#pragma once

#include "IntegerBaseNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unary operators on integers.
			class IntegerUnaryOperatorNode : public IntegerBaseNode<UnarySymbolNode> {
			public:
				/// The unary operation.
				KiwiOld::InstructionType op;

				/// Constructor.
				IntegerUnaryOperatorNode(const Boxx::UByte size, const KiwiOld::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}