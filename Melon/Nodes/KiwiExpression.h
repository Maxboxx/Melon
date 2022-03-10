#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a kiwi argument.
		class KiwiExpression : public Expression {
		public:
			/// The kiwi argument.
			Kiwi::Argument argument;

			/// The absolute type name for the argument value.
			Symbols::NameList type;

			/// Create from kiwi argument.
			KiwiExpression(const Kiwi::Argument& arg);
			~KiwiExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}