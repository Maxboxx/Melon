#pragma once

#include "IntegerSymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {
			///B IntegerConvertNode
			/// Node for converting an integer to an integer
			class IntegerConvertNode : public IntegerSymbolNode {
			public:
				bool sign;
				Boxx::UByte targetSize;

				///T Compile
				///A nodes[0]: The integer value to convert to a integer
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}