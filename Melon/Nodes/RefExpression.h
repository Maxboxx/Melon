#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for references.
		class RefExpression : public Expression {
		public:
			/// The expression to reference.
			Ptr<Expression> expression;

			RefExpression(Ptr<Expression> expression);
			~RefExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};

		/// Node for weak references.
		class WeakRefExpression : public Expression {
		public:
			/// The expression to reference.
			Weak<Expression> expression;

			WeakRefExpression(Weak<Expression> expression);
			~WeakRefExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}