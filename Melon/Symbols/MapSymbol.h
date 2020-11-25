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
			Symbol* AddSymbol(const Boxx::String& name, Symbol* const symbol);

			///T Get Symbol
			/// Gets the symbol with the specified name
			/// Returns <code>nullptr</code> if the symbol does not exist
			Symbol* operator[](const Boxx::String& name) const;

		protected:
			Boxx::Map<Boxx::String, Symbol*> symbols;
		};
	}
}
