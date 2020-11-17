#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B MemoryNode
		/// Node for a memory location
		class MemoryNode : public Node {
		public:

			///T Memory Location
			Kiwi::MemoryLocation mem;

			///T Type
			/// Absolute type name for the stack value
			Symbols::ScopeList type;

			///T Constructor
			MemoryNode(const Boxx::Int offset);
			MemoryNode(const Kiwi::MemoryLocation& mem);
			~MemoryNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}