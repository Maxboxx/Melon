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

			///T Get Scope
			/// Gets the scope with the specified index
			/// Returns <code>nullptr</code> if the scope does not exist
			SymbolTable* operator[](const Boxx::UInt index) const;

			///T Index
			/// Gets the index of the scope
			Boxx::UInt Index() const;

			///T Name
			/// Gets the name of the scope
			virtual Scope Name() const override;

		private:
			Boxx::List<SymbolTable*> scopes;
			Boxx::UInt index = 0;

			static SymbolTable globalTable;
		};
	}
}
