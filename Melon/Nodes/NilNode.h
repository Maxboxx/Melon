#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a {nil} value.
		class NilNode : public ExpressionNode {
		public:
			NilNode(const FileInfo& file);
			~NilNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}