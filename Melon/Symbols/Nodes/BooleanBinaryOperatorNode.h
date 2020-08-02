#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h";

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B BooleanBinaryOperatorNode
			/// Node for binary operatorn between boolean values
			class BooleanBinaryOperatorNode : public SymbolNode {
			public:

				///T Operator
				Kiwi::InstructionType op;

				///T Contructor
				BooleanBinaryOperatorNode(const Kiwi::InstructionType name);

				///T Compile
				///A nodes[0]: The first operand
				///A nodes[1]: The second operand
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}