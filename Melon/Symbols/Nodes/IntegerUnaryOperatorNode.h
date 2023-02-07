#pragma once

#include "IntegerBaseNode.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for unary operators on integers.
			class IntegerUnaryOperatorNode : public IntegerBaseNode<UnarySymbolNode> {
			public:
				/// The binary operator.
				Symbols::Name op;

				/// Constructor.
				IntegerUnaryOperatorNode(const Symbols::Name& op);

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}