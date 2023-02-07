#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for converting an integer to an integer.
			class IntegerConvertNode : public UnarySymbolNode {
			public:
				/// The integer type to convert to.
				Kiwi::Type type;

				IntegerConvertNode(const Kiwi::Type& type);

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}