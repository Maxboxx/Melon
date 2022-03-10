#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a stack position.
		class StackExpression : public Expression {
		public:
			/// The stack position.
			Boxx::UInt stack;

			/// The absolute type name of the node.
			Symbols::NameList type;

			StackExpression(const Boxx::UInt stack);
			~StackExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}