#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ArgumentNode
		/// Node for a kiwi arguement
		class ArgumentNode : public Node {
		public:

			///T Argument
			Kiwi::Argument argument;

			///T Type
			/// Absolute type name for the stack value
			Symbols::ScopeList type;

			///T Constructor
			ArgumentNode(const Kiwi::Argument& arg);
			~ArgumentNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}