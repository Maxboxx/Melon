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

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}