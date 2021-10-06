#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the dot operator.
		class DotNode : public ExpressionNode {
		public:
			/// The expression to perform the dot operation on.
			Expression expression;

			/// The name of the dot operator.
			Symbols::Name name;

			DotNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DotNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Expression Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}