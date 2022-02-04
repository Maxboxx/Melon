#pragma once

#include "BinaryOperatorNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the default operator.
		class DefaultNode : public BinaryOperatorNode {
		public:
			DefaultNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DefaultNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual _Expression_ Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}