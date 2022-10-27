#pragma once

#include "Ptr.h"

#include "Boxx/Pointer.h"

#include "Symbol.h"
#include "ScopeSymbol.h"

namespace Melon {
	namespace Nodes {
		class RootNode;
	}

	///N Melon::Symbols
	namespace Symbols {
		class IntegerSymbol;
		class VariableSymbol;
		class FunctionSymbol;

		/// A table containing all symbols.
		class SymbolTable {
		public:
			///H Absolute Search

			/// Finds a symbol of a specific name in the table.
			static Symbol* FindAbsolute(const NameList& name, const FileInfo& file);

			/// Checks if a symbol of a specific name exists in the table.
			///R The symbol if it exists or {nullptr} if it does not.
			static Symbol* ContainsAbsolute(const NameList& name);

			/// Finds a symbol of a specific name and symbol type in the table.
			///M
			template <class T>
			static T* FindAbsolute(const NameList& name, const FileInfo& file);
			///M

			/// Checks if a symbol of a specific name and symbol type exists in the table.
			///R The symbol if it exists or {nullptr} if it does not.
			///M
			template <class T>
			static T* ContainsAbsolute(const NameList& name);
			///M

			///H Relative Search

			/// Options for searching for symbols.
			enum class SearchOptions : Boxx::UByte {
				/// No option.
				None = 0,

				/// Accepts symbols that appear after the current statement number.
				IgnoreOrder = 1,

				/// Ignores symbols within included namespaces.
				IgnoreIncludedNamespaces = 2,

				/// Replaces the templates for both the scope and name with absolute values instead.
				ReplaceTemplates = 4
			};

			/// Finds a symbol of a specific name in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			static Symbol* Find(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			/// Finds a symbol of a specific name in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			static Symbol* Find(NameList name, NameList scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			/// Checks if a symbol of a specific name exists in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///R The symbol if it exists or {nullptr} if it does not.
			static Symbol* Contains(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			/// Checks if a symbol of a specific name exists in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///R The symbol if it exists or {nullptr} if it does not.
			static Symbol* Contains(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);

			/// Finds a symbol of a specific name and symbol type in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///M
			template <class T>
			static T* Find(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			/// Finds a symbol of a specific name and symbol type in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///M
			template <class T>
			static T* Find(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			/// Checks if a symbol of a specific name and symbol type exists in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///R The symbol if it exists or {nullptr} if it does not.
			///M
			template <class T>
			static T* Contains(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			/// Checks if a symbol of a specific name and symbol type exists in the table.
			///A name: The relative name of the symbol.
			///A scope: The scope to find the symbol from.
			///A file: Information about the file to search from.
			///A options: Search options to use for finding the symbol.
			///R The symbol if it exists or {nullptr} if it does not.
			///M
			template <class T>
			static T* Contains(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options = SearchOptions::None);
			///M

			/// Finds the operator function to use for the specified types.
			///A op: The operator name.
			///A type1: The type of the first operand.
			///A type2: The type of the second operand.
			///R The operator function to use.
			static FunctionSymbol* FindOperator(const Name& op, TypeSymbol* const type1, TypeSymbol* const type2, const FileInfo& file);

			/// Finds the implicit conversion operator for the specified types.
			///A from: The type to convert from.
			///A to:   The type to convert to.
			///R The conversion operator.
			/// {nullptr} if the operator was not found.
			static FunctionSymbol* FindImplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file);

			/// Finds the explicit conversion operator for the specified types.
			///A from: The type to convert from.
			///A to:   The type to convert to.
			///R The conversion operator.
			/// {nullptr} if the operator was not found.
			static FunctionSymbol* FindExplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file);

			/// Adds basic types to the symbol table.
			static void Setup();

			/// Specializes a template symbol.
			///p The symbols are specialized during include scan.
			static void SpecializeTemplate(const NameList& name, Symbol* const scope, const FileInfo& file); 

			///H Basic Types
			
			/// The symbol for the {byte} type.
			static IntegerSymbol* Byte;

			/// The symbol for the {tiny} type.
			static IntegerSymbol* Tiny;

			/// The symbol for the {utiny} type.
			static IntegerSymbol* UTiny;

			/// The symbol for the {short} type.
			static IntegerSymbol* Short;

			/// The symbol for the {ushort} type.
			static IntegerSymbol* UShort;

			/// The symbol for the {int} type.
			static IntegerSymbol* Int;

			/// The symbol for the {uint} type.
			static IntegerSymbol* UInt;

			/// The symbol for the {long} type.
			static IntegerSymbol* Long;

			/// The symbol for the {ulong} type.
			static IntegerSymbol* ULong;

			/// The symbol for the {bool} type.
			static IntegerSymbol* Bool;

			/// The symbol for the {nil} type.
			static TypeSymbol* Nil;

		private:
			static Symbol* FindInNamespaces(const NameList& name, const FileInfo& file);

			static NameList ReplaceTemplatesAbsolute(const NameList& name, const FileInfo& file);
			static NameList ReplaceTemplates(const NameList& name, const NameList& scope, const FileInfo& file);

			static Boxx::Pointer<ScopeSymbol> symbols;

			static void SetupIntegers();
			static void SetupBoolean();
			static void SetupNil();
			static void SetupOptional();

			friend Melon::Nodes::RootNode;
			friend Symbol;
			friend FunctionSymbol;
			friend VariableSymbol;

			struct TemplateInfo {
				NameList name;
				Symbol* scope;
				FileInfo file;
			};

			static Boxx::List<TemplateInfo> templateSymbols;
		};

		BOXX_ENUM_FLAGS(SymbolTable::SearchOptions);

		template <class T>
		inline T* SymbolTable::FindAbsolute(const NameList& name, const FileInfo& file) {
			if (name.Size() > 0 && name[0] == Name::Global) {
				NameList list;

				for (Boxx::UInt i = 1; i < name.Size(); i++) {
					list = list.Add(name[i]);
				}

				return symbols->Symbol::Find<T>(list, file);
			}

			return symbols->Symbol::Find<T>(name, file);
		}

		template <class T>
		inline T* SymbolTable::ContainsAbsolute(const NameList& name) {
			if (name.Size() > 0 && name[0] == Name::Global) {
				NameList list;

				for (Boxx::UInt i = 1; i < name.Size(); i++) {
					list = list.Add(name[i]);
				}

				return symbols->Symbol::Contains<T>(list);
			}

			return symbols->Symbol::Contains<T>(name);
		}

		template <class T>
		inline T* SymbolTable::Find(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
			return SymbolTable::Find<T>(NameList().Add(name), scope, file, options);
		}

		template <class T>
		inline T* SymbolTable::Find(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
			if (Symbol* const sym = Find(name, scope, file, options)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* SymbolTable::Contains(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
			return SymbolTable::Contains<T>(NameList().Add(name), scope, file, options);
		}

		template <class T>
		inline T* SymbolTable::Contains(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
			if (Symbol* const sym = Contains(name, scope, file, options)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}
	}
}
