#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for assigning classes.
			class ClassAssignNode : public BinarySymbolNode {
			public:
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}