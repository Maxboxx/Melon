#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B SafeUnwrapEndNode
		/// End node for the safe unwrap opertator
		class SafeUnwrapEndNode : public Node {
		public:

			///T Node
			/// The unwrapped node
			NodePtr node;

			SafeUnwrapEndNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SafeUnwrapEndNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String jumpInstName;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}