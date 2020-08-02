#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B BooleanUnaryOperatorNode
			/// Node for unary operators on boolean values
			class BooleanUnaryOperatorNode : public SymbolNode {
			public:

				///T Operator
				Kiwi::InstructionType op;

				///T Constructor
				BooleanUnaryOperatorNode(const Kiwi::InstructionType name);

				///T Compile
				///A nodes[0]: The value to perform the operation on
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}