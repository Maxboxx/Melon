#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for casting classes.
			class ClassCastNode : public UnarySymbolNode {
			public:
				TypeSymbol* type;

				ClassCastNode(TypeSymbol* type) {
					this->type = type;
				}

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}