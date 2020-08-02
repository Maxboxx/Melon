#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B EmptyNode
		/// A node that does not get compiled
		class EmptyNode : public Node {
		public:

			///T node
			/// This node does not get compiled but the <code>ToMango()</code> method is called
			NodePtr node;

			///T Constructor
			EmptyNode();
			~EmptyNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}