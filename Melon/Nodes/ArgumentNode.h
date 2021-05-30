#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a kiwi argument.
		class ArgumentNode : public ExpressionNode {
		public:
			/// The kiwi argument.
			Kiwi::Argument argument;

			/// The absolute type name for the argument value.
			Symbols::NameList type;

			/// Create from kiwi argument.
			ArgumentNode(const Kiwi::Argument& arg);
			~ArgumentNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}