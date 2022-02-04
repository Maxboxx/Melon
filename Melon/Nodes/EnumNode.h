#pragma once

#include "StatementNode.h"

namespace Melon {
	namespace Symbols {
		class EnumSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		/// Node for enums.
		class EnumNode : public Statement {
		public:
			/// The name of the enum.
			Symbols::Name name;

			/// The values of the enum.
			Boxx::List<Symbols::Name> values;

			/// The enum symbol.
			Symbols::EnumSymbol* symbol;

			EnumNode(Symbols::Symbol* const scope, const FileInfo& file);
			~EnumNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}