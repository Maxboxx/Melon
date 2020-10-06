#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B TypeNode
		/// A node that contains a type
		class TypeNode : public Node {
		public:

			///T Type
			Symbols::ScopeList type;

			///T Constructor
			TypeNode(const Symbols::ScopeList& type);
			~TypeNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Symbols::ScopeList Type() const override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
