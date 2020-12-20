#pragma once

#include "Boxx/Pointer.h"

#include "Symbol.h"
#include "ScopeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B SymbolTable
		/// A table containing all symbols
		class SymbolTable {
		public:
			///T Find
			/// Finds a symbol of a specific name in the table 
			static Symbol* Find(const ScopeList& name, const FileInfo& file);

			///T Contains
			/// Checks if a symbol of a specific name exists in the table
			///R Symbol*: The symbol if it exists or <code>nullptr</code> if it does not
			static Symbol* Contains(const ScopeList& name);

			///T Find
			/// Finds a symbol of a specific name and symbol type in the table 
			///M
			template <class T>
			static T* Find(const ScopeList& name, const FileInfo& file);
			///M

			///T Contains
			/// Checks if a symbol of a specific name and symbol type exists in the table
			///R Symbol*: The symbol if it exists or <code>nullptr</code> if it does not
			///M
			template <class T>
			static T* Contains(const ScopeList& name);
			///M

		private:
			static Boxx::Pointer<ScopeSymbol> symbols;
		};
	}
}
