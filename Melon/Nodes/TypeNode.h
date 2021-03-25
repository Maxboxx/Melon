#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node that contains a type.
		class TypeNode : public Node {
		public:
			/// The absolute type.
			Symbols::ScopeList type;

			TypeNode(const Symbols::ScopeList& type);
			~TypeNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
