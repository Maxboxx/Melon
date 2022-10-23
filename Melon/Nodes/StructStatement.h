#pragma once

#include "Statement.h"

#include "Melon/Symbols/StructSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for structs.
		class StructStatement : public Statement {
		public:
			/// The name of the struct.
			Symbols::Name name;

			/// The members of the struct.
			Boxx::List<Symbols::Name> vars;

			/// The struct symbol.
			Symbols::StructSymbol* symbol;

			StructStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~StructStatement();

			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;

		private:
			bool IsRecursive(Symbols::StructSymbol* const symbol) const;
		};
	}
}
