#pragma once

#include "SymbolNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for comparing boolean values.
			class [[deprecated]] BooleanCompareNode : public BinarySymbolNode {
			public:
				/// The compare operator.
				KiwiOld::InstructionType op;

				/// Constructor.
				BooleanCompareNode(const KiwiOld::InstructionType op);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}