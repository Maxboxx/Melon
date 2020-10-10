#pragma once

#include "SymbolNode.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B BooleanCompareNode
			/// Node for comparing boolean values
			class BooleanCompareNode : public SymbolNode {
			public:
				///T Operator
				Kiwi::InstructionType op;

				///T Constructor
				BooleanCompareNode(const Kiwi::InstructionType op);

				///T Compile
				///A nodes[0]: The first operand
				///A nodes[1]: The second operand
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}