#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for the discard keyword.
		class DiscardNode : public ExpressionNode {
		public:
			DiscardNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DiscardNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}