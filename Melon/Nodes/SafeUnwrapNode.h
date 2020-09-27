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

			SafeUnwrapNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~SafeUnwrapNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Symbols::Symbol GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}