#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
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

			TypeConversion(Symbols::Symbol* const scope, const FileInfo& file);
			~TypeConversion();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
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