#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B NilNode
		/// Node for a nil value
		class NilNode : public Node {
		public:
			NilNode(const FileInfo& file);
			~NilNode();

			virtual Symbols::ScopeList Type() const override;
			virtual bool IsImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}