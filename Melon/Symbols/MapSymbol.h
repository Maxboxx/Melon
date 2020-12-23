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

		template <class T>
		inline T* MapSymbol::AddSymbol(const Scope& name, T* const symbol) {
			try {
				symbols.Add(name, symbol);
				symbol->name = name;
				symbol->parent = this;
				return symbol;
			}
			catch (Boxx::MapKeyError& e) {
				if (symbol->parent == nullptr) delete symbol;
				ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + name.ToString() + SymbolError::RedefinitionEnd, symbol->file));
				return nullptr;
			}
		}
	}
}
