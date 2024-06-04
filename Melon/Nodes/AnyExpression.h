#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the {any} keyword.
		class AnyExpression : public Expression {
		public:
			/// Creates an any expression.
			AnyExpression(const FileInfo& file);
			~AnyExpression();

			virtual Symbols::Symbol* Symbol(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		private:
			Symbols::TypeSymbol** inferred = new Symbols::TypeSymbol*(nullptr);
		};
	}
}