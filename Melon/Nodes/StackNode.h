#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a stack position.
		class StackNode : public ExpressionNode {
		public:
			/// The stack position.
			Boxx::UInt stack;

			/// The absolute type name of the node.
			Symbols::NameList type;

			StackNode(const Boxx::UInt stack);
			~StackNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}