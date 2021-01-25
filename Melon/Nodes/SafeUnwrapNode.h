#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B SafeUnwrapNode
		/// Node for the safe unwrap opertator
		class SafeUnwrapNode : public Node {
		public:

			///T Node
			/// Node to perform the unwrap operation on
			NodePtr node;

			SafeUnwrapNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SafeUnwrapNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}