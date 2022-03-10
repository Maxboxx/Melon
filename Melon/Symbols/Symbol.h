#pragma once

#include "Types.h"

#include "Boxx/String.h"
#include "Boxx/Set.h"
#include "Boxx/List.h"
#include "Boxx/Map.h"
#include "Boxx/ReplacementMap.h"

#include "NameList.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Nodes {
		class RootNode;
	}

	///N Melon::Symbols
	namespace Symbols {
		class MapSymbol;
		class TypeSymbol;
		class FunctionSymbol;
		class SymbolTable;

		/// Contains information about a symbol.
		class Symbol {
		public:
			Symbol(const FileInfo& file);
			~Symbol();

			/// Gets the parent symbol.
			Symbol* Parent() const;

			/// Gets the parent symbol if the parent is of the specified type.
			///M
			template <class T>
			T* Parent() const;
			///M

			/// Gets the parent type of the symbol.
			TypeSymbol* ParentType() const;

			/// Gets the parent type or the symbol if the symbol is a type symbol.
			TypeSymbol* CurrentType();

			/// Gets the parent function of the symbol.
			FunctionSymbol* ParentFunction() const;

			/// Gets the parent function or the symbol if the symbol is a function symbol.
			FunctionSymbol* CurrentFunction();

			/// Gets the name of the symbol.
			virtual Symbols::Name Name();

			/// Gets the absolute name of the symbol.
			virtual NameList AbsoluteName();

			/// Gets the type of the symbol.
			virtual TypeSymbol* Type();

			/// Specializes a template symbol.
			virtual Symbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) = 0;

			/// Checks if the symbol or any of its parents has template arguments that have not been specialized.
			virtual bool IsNotSpecialized();

			/// Sets the template types of the symbol to the types of the specified symbol.
			///p The supplied symbol has to be a specialization of the current symbol.
			virtual void SetTemplateValues(Symbol* const symbol);

			/// Finds a specific symbol in the current symbol.
			Symbol* Find(const Symbols::Name& name, const FileInfo& file);

			/// Finds a specific symbol in the current symbol.
			Symbol* Find(const NameList& nameList, const FileInfo& file);

			/// Finds a specific symbol of the specified type.
			///M
			template <class T>
			T* Find(const Symbols::Name& name, const FileInfo& file);
			///M

			/// Finds a specific symbol of the specified type.
			///M
			template <class T>
			T* Find(const NameList& name, const FileInfo& file);
			///M

			/// Checks if the specified symbol exists and returns it.
			///p If the symbol was not found {nullptr} is returned.
			Symbol* Contains(const Symbols::Name& name);

			/// Checks if the specified symbol exists and returns it.
			///p If the symbol was not found {nullptr} is returned.
			Symbol* Contains(const NameList& nameList);

			/// Checks if a specific symbol of the specified type exists and returns it.
			///p If the symbol was not found {nullptr} is returned
			///M
			template <class T>
			T* Contains(const Symbols::Name& scope);
			///M

			/// Checks if a specific symbol of the specified type exists and returns it.
			///p If the symbol was not found {nullptr} is returned
			///M
			template <class T>
			T* Contains(const NameList& name);
			///M

			/// Checks if the symbol is a specific type of symbol.
			///M
			template <class T>
			bool Is() {
			///M
				return dynamic_cast<T*>(this) != nullptr;
			}

			/// Casts the symbol to a specific symbol type.
			///p Returns {nullptr} if the cast fails.
			///M
			template <class T>
			T* Cast() {
			///M
				return dynamic_cast<T*>(this);
			}

			/// Gets the file info of the symbol.
			FileInfo File() const;

			/// Converts the absolute name of the symbol to a simple string.
			Boxx::String ToString();

		protected:
			static TypeSymbol* ReplaceType(TypeSymbol* const type, const Boxx::ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file);
			static NameList ReplaceTypeScope(TypeSymbol* const type, const Boxx::ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file);

			virtual Symbol* FindSymbol(const NameList& nameList, const Boxx::UInt index, const FileInfo& file);

			void FindError(const NameList& nameList, const Boxx::UInt index, const FileInfo& file);

			FileInfo file;
			Symbol* parent = nullptr;
			Symbols::Name name;

			friend MapSymbol;
			friend SymbolTable;
		};

		template <class T>
		inline T* Symbol::Parent() const {
			return parent->Cast<T>();
		}

		template <class T>
		inline T* Symbol::Find(const Symbols::Name& name, const FileInfo& file) {
			if (Symbol* const sym = Find(NameList(name), file)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Find(const NameList& nameList, const FileInfo& file) {
			if (Symbol* const sym = Find(nameList, file)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Contains(const Symbols::Name& name) {
			if (Symbol* const sym = Contains(NameList(name))) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Contains(const NameList& nameList) {
			ErrorLog::AddMarker();
			Symbol* const sym = FindSymbol(nameList, 0, FileInfo());
			ErrorLog::Revert();

			if (sym) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}
	}
}
