#pragma once

#include "IntegerSymbolNode.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B IntegerBinaryOperatorNode
			/// Node for binary operators between integers
			class IntegerBinaryOperatorNode : public IntegerSymbolNode {
			public:

				///T Operator
				Kiwi::InstructionType op;

				///T Signed result
				bool sign;

				///T Constructor
				IntegerBinaryOperatorNode(const Boxx::UByte size, const bool sign, const Kiwi::InstructionType name);

				///T Compile
				///A nodes[0]: The first operand
				///A nodes[1]: The second operand
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}