#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for enum comparison.
			class EnumCompareNode : public BinarySymbolNode {
			public:
				Name op;

				EnumCompareNode(const Name& op) {
					this->op = op;
				}

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}