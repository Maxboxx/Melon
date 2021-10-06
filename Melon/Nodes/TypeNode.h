#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node that contains a type.
		class TypeNode : public ExpressionNode {
		public:
			/// The absolute type name.
			Symbols::NameList type;

			TypeNode(const Symbols::NameList& type);
			~TypeNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
