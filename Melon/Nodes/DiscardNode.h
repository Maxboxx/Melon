#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B DiscardNode
		/// Node for the discard keyword
		class DiscardNode : public Node {
		public:
			///T Constructor
			DiscardNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~DiscardNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}