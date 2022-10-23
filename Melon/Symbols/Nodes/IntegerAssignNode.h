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

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const override;
			};
		}
	}
}

