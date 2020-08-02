#pragma once

#include "IntegerSymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B IntegerAssignNode
			/// Node for integer assignments
			class IntegerAssignNode : public IntegerSymbolNode {
			public:

				///T Constructor
				IntegerAssignNode(const Boxx::UByte size);

				///T Compile
				///A nodes[0]: The node to assign a value to
				///A nodes[1]: The value to be assigned
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}