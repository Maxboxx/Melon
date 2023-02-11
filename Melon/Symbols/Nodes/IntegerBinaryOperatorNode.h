#pragma once

#include "SymbolNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for binary operators between integers.
			class IntegerBinaryOperatorNode : public BinarySymbolNode {
			public:
				/// The binary operator.
				Symbols::Name op;

				/// The type of the operation.
				Kiwi::Type type;

				/// Constructor.
				IntegerBinaryOperatorNode(const Symbols::Name& op, const Kiwi::Type& type);

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}