#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a memory location.
		class MemoryNode : public Node {
		public:
			/// The memory location.
			Kiwi::MemoryLocation mem;

			/// The absolute type name of the node.
			Symbols::NameList type;

			MemoryNode(const Boxx::Int offset);
			MemoryNode(const Kiwi::MemoryLocation& mem);
			~MemoryNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}