#pragma once

#include "Boxx/Pointer.h"

#include "Symbol.h"
#include "ScopeSymbol.h"

namespace Melon {
	namespace Symbols {
		class IntegerSymbol;

		///B SymbolTable
		/// A table containing all symbols
		class SymbolTable {
		public:
			///H Absolute Search

			///T Find Absolute
			/// Finds a symbol of a specific name in the table 
			static Symbol* FindAbsolute(const ScopeList& name, const FileInfo& file);

			///T Contains Absolute
			/// Checks if a symbol of a specific name exists in the table
			///R Symbol*: The symbol if it exists or <code>nullptr</code> if it does not
			static Symbol* ContainsAbsolute(const ScopeList& name);

			///T Find Absolute Cast
			/// Finds a symbol of a specific name and symbol type in the table 
			///M
			template <class T>
			static T* FindAbsolute(const ScopeList& name, const FileInfo& file);
			///M

			///T Contains Absolute Cast
			/// Checks if a symbol of a specific name and symbol type exists in the table
			///R T*: The symbol if it exists or <code>nullptr</code> if it does not
			///M
			template <class T>
			static T* ContainsAbsolute(const ScopeList& name);
			///M

			///H Relative Search

			////B SearchOptions
			//// Options for searching for symbols
			enum class SearchOptions : Boxx::UByte {
				////H Values

				////T None
				None = 0,

				////T IgnoreOrder
				//// Accepts symbols that appear after the current statement number
				IgnoreOrder = 1,

				////T IgnoreIncludedNamespaces
				//// Ignores symbols within included namespaces
				IgnoreIncludedNamespaces = 2,

				////T ReplaceTemplates
				//// Replaces the templates for both the scope and name with absolute values instead
				ReplaceTemplates = 4
			};

			///T Find
			/// Finds a symbol of a specific name in the table
			///A name: The relative name of the symbol
			///A scope: The scope to find the symbol from
			///A file: Information about the file to search from
			///A options: Search options to use for finding the symbol
			static Symbol* Find(ScopeList name, ScopeList scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			///T Contains
			/// Checks if a symbol of a specific name exists in the table
			///A name: The relative name of the symbol
			///A scope: The scope to find the symbol from
			///A file: Information about the file to search from
			///A options: Search options to use for finding the symbol
			///R Symbol*: The symbol if it exists or <code>nullptr</code> if it does not
			static Symbol* Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			///T Find
			/// Finds a symbol of a specific name and symbol type in the table
			///A name: The relative name of the symbol
			///A scope: The scope to find the symbol from
			///A file: Information about the file to search from
			///A options: Search options to use for finding the symbol
			///M
			template <class T>
			static T* Find(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			///T Contains
			/// Checks if a symbol of a specific name and symbol type exists in the table
			///A name: The relative name of the symbol
			///A scope: The scope to find the symbol from
			///A file: Information about the file to search from
			///A options: Search options to use for finding the symbol
			///R T*: The symbol if it exists or <code>nullptr</code> if it does not
			///M
			template <class T>
			static T* Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			///T Setup
			/// Adds basic types to the symbol table
			static void Setup();

			///T Basic types
			///M
			static IntegerSymbol* Byte;
			static IntegerSymbol* UByte;
			static IntegerSymbol* Short;
			static IntegerSymbol* UShort;
			static IntegerSymbol* Int;
			static IntegerSymbol* UInt;
			static IntegerSymbol* Long;
			static IntegerSymbol* ULong;
			static IntegerSymbol* Bool;
			static TypeSymbol*    Nil;
			///M

		private:
			static Symbol* FindInNamespaces(const ScopeList& name, const FileInfo& file);

			static ScopeList ReplaceTemplatesAbsolute(const ScopeList& name, const FileInfo& file);
			static ScopeList ReplaceTemplates(const ScopeList& name, const ScopeList& scope, const FileInfo& file);

			static Boxx::Pointer<ScopeSymbol> symbols;

			static void SetupIntegers();
			static void SetupBoolean();
			static void SetupNil();
			static void SetupOptional();
		};

		BOXX_ENUM_FLAGS(SymbolTable::SearchOptions);

		template <class T>
		inline T* SymbolTable::FindAbsolute(const ScopeList& name, const FileInfo& file) {
			if (name.Size() > 0 && name[0] == Scope::Global) {
				ScopeList list;

				for (Boxx::UInt i = 1; i < name.Size(); i++) {
					list = list.Add(name[i]);
				}

				return symbols->Symbol::Find<T>(list, file);
			}

			return symbols->Symbol::Find<T>(name, file);
		}

		template <class T>
		inline T* SymbolTable::ContainsAbsolute(const ScopeList& name) {
			if (name.Size() > 0 && name[0] == Scope::Global) {
				ScopeList list;

				for (Boxx::UInt i = 1; i < name.Size(); i++) {
					list = list.Add(name[i]);
				}

				return symbols->Symbol::Contains<T>(list);
			}

			return symbols->Symbol::Contains<T>(name);
		}

		template <class T>
		inline T* SymbolTable::Find(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
			if (Symbol* const sym = Find(name, scope, file, options)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* SymbolTable::Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
			if (Symbol* const sym = Contains(name, scope, file, options)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}
	}
}
