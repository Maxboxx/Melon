#pragma once

#include "Symbol.h"

namespace Melon {
	namespace Symbols {
		///B MapSymbol
		/// Contains a map of all sub symbols of the symbol
		class MapSymbol : public Symbol {
		public:
			MapSymbol(const FileInfo& file);
			~MapSymbol();

			///T Add Symbol
			/// Adds a symbol to the table
			///R Symbol*: The new symbol
			/// <code>nullptr</code> if the symbol already exists
			///M
			template <class T>
			T* AddSymbol(const Scope& name, T* const symbol);
			///M

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			Boxx::Map<Scope, Symbol*> symbols;
		};
	}
}
