#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B StackNode
		/// Node for a stack location
		class StackNode : public Node {
		public:

			///T Stack Location
			Boxx::UInt stack;

			///T Type
			/// Absolute type name of the node
			Symbols::ScopeList type;

			///T Constructor
			StackNode(const Boxx::UInt stack);
			~StackNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}