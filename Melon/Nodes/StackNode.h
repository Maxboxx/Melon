#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B StackNode
		/// Node for a stack offset
		class StackNode : public Node {
		public:

			///T Stack offset
			Boxx::Int stack;

			///T Type
			/// Absolute type name for the stack value
			Symbols::ScopeList type;

			Boxx::Optional<Boxx::UInt> regIndex;

			///T Constructor
			StackNode(const Boxx::Int stack);
			StackNode(const Boxx::UInt regIndex, const Boxx::Int stack);
			StackNode(const Kiwi::MemoryLocation& mem);
			~StackNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}