#pragma once

#include "SymbolNode.h"

#include "Kiwi/Old/Kiwi.h";

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between boolean values.
			class BooleanBinaryOperatorNode : public BinarySymbolNode {
			public:
				/// The operator.
				Symbols::Name op;

				/// Constructor with operator.
				BooleanBinaryOperatorNode(const Symbols::Name& op);

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}