#pragma once

#include "SymbolNode.h"

#include "Kiwi/Old/Kiwi.h";

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between boolean values.
			class BooleanBinaryOperatorNode : public BinarySymbolNode {
			public:
				/// The operator.
				KiwiOld::InstructionType op;

				/// Constructor with operator.
				BooleanBinaryOperatorNode(const KiwiOld::InstructionType op);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}