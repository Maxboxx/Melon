#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the {is} expression.
		class IsExpression : public Expression {
		public:
			/// The expression.
			Ptr<Expression> expression;

			/// The type to check.
			Symbols::NameList type;

			/// Creates an is expression.
			IsExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~IsExpression();

			/// The symbol of the type expression.
			Symbols::Symbol* TypeSymbol() const;

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}