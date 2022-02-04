#pragma once

#include "StatementNode.h"

#include "Melon/Symbols/StructSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for structs.
		class StructNode : public Statement {
		public:
			/// The name of the struct.
			Symbols::Name name;

			/// The members of the struct.
			Boxx::List<Symbols::Name> vars;

			/// The struct symbol.
			Symbols::StructSymbol* symbol;

			StructNode(Symbols::Symbol* const scope, const FileInfo& file);
			~StructNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;

		private:
			bool IsRecursive(Symbols::StructSymbol* const symbol) const;
		};
	}
}
