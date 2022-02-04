#pragma once

#include "IntegerBaseNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for integer assignments.
			class IntegerAssignNode : public IntegerBaseNode<BinarySymbolNode> {
			public:
				/// Constructor
				IntegerAssignNode(const Boxx::UByte size);

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::_Expression_& operand1, const Melon::Nodes::_Expression_& operand2, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}

