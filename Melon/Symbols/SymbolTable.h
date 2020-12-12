#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class ScopeList;

		class Symbol;

		///B SymbolTable
		/// A table containing all symbols and sub scopes of a specific scope
		/// All pointers returned by functions will automatically be deleted when the symbol table is deleted
		class SymbolTable final : public MapSymbol {
		public:
			SymbolTable(const FileInfo& file);
			~SymbolTable();

			///T Add Scope
			/// Adds a new sub scope to the table
			///R SymbolTable*: The table for the new scope
			SymbolTable* AddScope(const FileInfo& file);

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

		private:
			Boxx::List<SymbolTable*> scopes;

			static SymbolTable globalTable;
		};
	}
}
