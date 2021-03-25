#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a boolean value.
		class BooleanNode : public Node {
		public:
			/// The boolean value.
			bool boolean = false;

			/// Creates a boolean node.
			BooleanNode(const FileInfo& file);
			~BooleanNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}