#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B OptionalAssignValueNode
			/// Node for assigning values to optional types
			class OptionalAssignValueNode : public SymbolNode {
			public:

				///T Compile
				///A nodes[0]: The node to assign a value to
				///A nodes[1]: The value to be assigned 
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}