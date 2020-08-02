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

			///T Constructor
			StackNode(const Boxx::Int stack);
			~StackNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}