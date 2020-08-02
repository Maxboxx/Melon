#pragma once

#include "Melon/Nodes/Node.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B SymbolNode
			/// Base for symbol nodes
			class SymbolNode {
				public:

				///T Compile
				/// Compiles the symbol node
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const = 0;
			};
		}
	}
}