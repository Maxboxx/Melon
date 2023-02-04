#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// End node for the safe unwrap opertator.
		class SafeUnwrapChain : public Expression {
		public:
			/// The unwrapped expression.
			Ptr<Expression> expression;

			SafeUnwrapChain(Symbols::Symbol* const scope, const FileInfo& file);
			~SafeUnwrapChain();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String jumpInstName;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}