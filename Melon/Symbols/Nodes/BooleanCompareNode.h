#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for comparing boolean values.
			class BooleanCompareNode : public BinarySymbolNode {
			public:
				/// The compare operator.
				Kiwi::InstructionType op;

				/// Constructor.
				BooleanCompareNode(const Kiwi::InstructionType op);

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand1, const Melon::Nodes::_Expression_& operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}