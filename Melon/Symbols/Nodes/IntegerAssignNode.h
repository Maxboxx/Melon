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

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}

