#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the discard keyword.
		class DiscardExpression : public Expression {
		public:
			DiscardExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~DiscardExpression();

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}