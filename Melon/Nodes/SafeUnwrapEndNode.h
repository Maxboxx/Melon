#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// End node for the safe unwrap opertator.
		class SafeUnwrapEndNode : public ExpressionNode {
		public:
			/// The unwrapped expression.
			Expression expression;

			SafeUnwrapEndNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SafeUnwrapEndNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Expression Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String jumpInstName;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}