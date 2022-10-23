#pragma once

#include "IntegerBaseNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between integers.
			class IntegerBinaryOperatorNode : public IntegerBaseNode<BinarySymbolNode> {
			public:
				/// The binary operation.
				KiwiOld::InstructionType op;

				/// {true} if the result should be signed.
				bool sign;

				/// Constructor.
				IntegerBinaryOperatorNode(const Boxx::UByte size, const bool sign, const KiwiOld::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}