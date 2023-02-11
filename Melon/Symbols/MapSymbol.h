#pragma once

#include "Symbol.h"

namespace Melon {
	namespace Nodes {
		class StructStatement;
	}

	///N Melon::Symbols
	namespace Symbols {
		/// A symbol with a map of all child symbols.
		class MapSymbol : public Symbol {
		public:
			MapSymbol(const FileInfo& file);
			~MapSymbol();

			/// Adds a child symbol.
			///[Template] T: Must inherit from {Symbol}.
			///R The new symbol.
			///p This value is {nullptr} if the symbol already exists.
			///M
			template <class T>
			T* AddSymbol(const Symbols::Name& name, T* const symbol);
			///M

			/// Adds a child symbol.
			///[Template] T: Must inherit from {Symbol}.
			///R The new symbol.
			///p This value is {nullptr} if the symbol already exists.
			///M
			template <class T>
			T* AddSymbol(const Symbols::Name& name, const Boxx::List<Symbols::Name>& aliases, T* const symbol);
			///M

			virtual MapSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			virtual Symbol* FindSymbol(const NameList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

			friend Melon::Nodes::StructStatement;

			Boxx::Map<Symbols::Name, Symbol*> symbols;
		};

		template <class T>
		inline T* MapSymbol::AddSymbol(const Symbols::Name& name, T* const symbol) {
			try {
				symbols.Add(name, symbol);
				symbol->name = name;
				symbol->parent = this;
				return symbol;
			}
			catch (Boxx::MapKeyError& e) {
				ErrorLog::Error(LogMessage("error.symbol.redefine", name.ToSimpleString()), symbol->file);

				if (symbol->parent == nullptr) delete symbol;
				return nullptr;
			}
		}

		template <class T>
		inline T* MapSymbol::AddSymbol(const Symbols::Name& name, const Boxx::List<Symbols::Name>& aliases, T* const symbol) {
			try {
				symbols.Add(name, symbol);
				symbol->name = name;
				symbol->aliases = aliases;
				symbol->parent = this;

				for (const Symbols::Name& name : aliases) {
					symbols.Add(name, symbol);
				}

				return symbol;
			}
			catch (Boxx::MapKeyError& e) {
				ErrorLog::Error(LogMessage("error.symbol.redefine", name.ToSimpleString()), symbol->file);

				if (symbol->parent == nullptr) delete symbol;
				return nullptr;
			}
		}
	}
}
