#pragma once

#include "Expression.h"

#include "Boxx/Optional.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for arrays.
		class IndexExpression : public Expression {
		public:
			/// The expression to index.
			Ptr<Expression> expression;

			/// The index args.
			Boxx::List<Ptr<Expression>> args;

			IndexExpression(Symbols::Symbol* scope, const FileInfo& file);
			~IndexExpression();

			Symbols::FunctionSymbol* GetFunc() const;

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}