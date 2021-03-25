#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node that does not get compiled.
		class EmptyNode : public Node {
		public:
			/// This node does not get compiled but the {ToMelon()} method is called for it.
			NodePtr node;

			EmptyNode();
			~EmptyNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}