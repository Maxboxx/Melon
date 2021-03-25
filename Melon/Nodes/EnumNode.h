#pragma once

#include "Node.h"

namespace Melon {
	namespace Symbols {
		class EnumSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		/// Node for enums.
		class EnumNode : public Node {
		public:
			/// The name of the enum.
			Symbols::Scope name;

			/// The values of the enum.
			Boxx::List<Symbols::Scope> values;

			/// The enum symbol.
			Symbols::EnumSymbol* symbol;

			EnumNode(Symbols::Symbol* const scope, const FileInfo& file);
			~EnumNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}