#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node that does not get compiled.
		class EmptyNode : public Statement {
		public:
			/// This statement does not get compiled but the {ToMelon()} method is called for it.
			_Statement_ statement = nullptr;

			EmptyNode();
			~EmptyNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
