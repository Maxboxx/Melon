#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for a boolean constant.
			class BooleanConstantNode : public ConstantSymbolNode {
			public:
				/// The boolean value.
				bool value;

				/// Constructor for a boolean value.
				BooleanConstantNode(const bool value);

				virtual Ptr<Kiwi::Value> Compile(Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}