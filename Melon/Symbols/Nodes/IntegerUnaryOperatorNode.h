#pragma once

#include "IntegerSymbolNode.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B IntegerUnaryOperator
			/// Node for unary operators on integers
			class IntegerUnaryOperatorNode : public IntegerSymbolNode {
			public:

				///T Operator
				Kiwi::InstructionType op;

				///T Constructor
				IntegerUnaryOperatorNode(const Boxx::UByte size, const Kiwi::InstructionType name);

				///T Compile
				///A nodes[0]: The value to perform the operation on
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}