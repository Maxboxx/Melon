#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for boolean assignment.
			class IndexGetNode : public SymbolNode {
			public:
				virtual Ptr<Kiwi::Value> Compile(Boxx::List<Weak<Melon::Nodes::Expression>> operands, Melon::Nodes::CompileInfo& info, bool includeType) const override;
			};
		}
	}
}