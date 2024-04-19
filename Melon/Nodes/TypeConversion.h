#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Symbols {
		class ValueSymbol;
	}

	namespace Nodes {
		/// Expression for converting a value to a different type.
		class TypeConversion : public Expression {
		public:
			/// The expression to convert to a different type.
			Ptr<Expression> expression;

			/// The absolute name of the type to convert to.
			Symbols::NameList type;

			/// {true} if the conversion is explicit.
			bool isExplicit = false;

			/// {true} if the conversion has an optional return type.
			bool isOptional = false;

			TypeConversion(Symbols::Symbol* const scope, const FileInfo& file);
			~TypeConversion();

			/// Gets the value symbol for the type if the type is an enum value.
			Symbols::ValueSymbol* GetValueSymbol() const;

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}