#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B StructNode
		/// Node for structs
		class StructNode : public Node {
		public:

			///T Name
			/// The name of the struct
			Symbols::Scope name;

			///T Variables
			/// The variables of the struct
			Boxx::List<Symbols::Scope> vars;

			///T Symbol
			Symbols::Symbol symbol;

			StructNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~StructNode();

			///T Is Recursive
			/// Checks if the struct is recursive in the specified symbol
			bool IsRecursive(const Symbols::Symbol& symbol) const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign) override;
		};
	}
}