#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// An expression that represents an empty value of a specific type.
		class TypeExpression : public Expression {
		public:
			/// The absolute type name.
			Symbols::NameList type;

			TypeExpression(const Symbols::NameList& type);
			~TypeExpression();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
