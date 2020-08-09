#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ContinueNode
		/// Node for continuing loops
		class ContinueNode : public Node {
		public:

			///T Loops
			/// The amount of loops to continue
			Boxx::UInt loops;

			ContinueNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ContinueNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}