#pragma once

#include "SymbolNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unary operators on boolean values.
			class BooleanUnaryOperatorNode : public UnarySymbolNode {
			public:
				/// The unary operation.
				KiwiOld::InstructionType op;

				/// Constructor.
				BooleanUnaryOperatorNode(const KiwiOld::InstructionType name);

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}