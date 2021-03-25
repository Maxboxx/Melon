#pragma once

#include "Node.h"

#include "Melon/Symbols/StructSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for structs.
		class StructNode : public Node {
		public:
			/// The name of the struct.
			Symbols::Scope name;

			/// The members of the struct.
			Boxx::List<Symbols::Scope> vars;

			/// The struct symbol.
			Symbols::StructSymbol* symbol;

			StructNode(Symbols::Symbol* const scope, const FileInfo& file);
			~StructNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign) override;

		private:
			bool IsRecursive(Symbols::StructSymbol* const symbol) const;
		};
	}
}
