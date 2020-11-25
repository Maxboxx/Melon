#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B EnumNode
		/// Node for structs
		class EnumNode : public Node {
		public:

			///T Name
			/// The name of the enum
			Symbols::Scope name;

			///T Variables
			/// The values of the enum
			Boxx::List<Symbols::Scope> values;

			///T Symbol
			Symbols::Symbols symbol;

			EnumNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~EnumNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}