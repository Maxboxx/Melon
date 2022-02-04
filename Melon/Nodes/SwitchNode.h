#pragma once

#include "StatementNode.h"
#include "SwitchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} statements.
		class SwitchNode : public SwitchBaseNode<Statement> {
		public:
			SwitchNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchNode();

			virtual bool IsScope() const override;
			virtual _Statement_ Optimize(OptimizeInfo& info) override;
		};
	}
}
