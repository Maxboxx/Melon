#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for references.
		class RefNode : public Expression {
		public:
			/// The expression to reference.
			Ptr<Expression> expression;

			RefNode(Ptr<Expression> expression);
			~RefNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};

		/// Node for weak references.
		class WeakRefNode : public Expression {
		public:
			/// The expression to reference.
			Weak<Expression> expression;

			WeakRefNode(Weak<Expression> expression);
			~WeakRefNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}