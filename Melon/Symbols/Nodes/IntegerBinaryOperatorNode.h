#pragma once

#include "IntegerBaseNode.h"

#include "Kiwi/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between integers.
			class IntegerBinaryOperatorNode : public IntegerBaseNode<BinarySymbolNode> {
			public:
				/// The binary operation.
				Kiwi::InstructionType op;

				/// {true} if the result should be signed.
				bool sign;

				/// Constructor.
				IntegerBinaryOperatorNode(const Boxx::UByte size, const bool sign, const Kiwi::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}