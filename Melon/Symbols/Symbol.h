#pragma once

#include "Boxx/String.h"
#include "Boxx/Set.h"
#include "Boxx/List.h"
#include "Boxx/Map.h"
#include "Boxx/ReplacementMap.h"

#include "ScopeList.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Nodes {
		class RootNode;
	}

	namespace Symbols {
		class MapSymbol;
		class TypeSymbol;
		class SymbolTable;

		///B Symbol
		/// Contains information about a symbol
		class Symbol {
		public:
			Symbol(const FileInfo& file);
			~Symbol();

			///T Parent
			/// Gets the parent symbol
			Symbol* Parent() const;

			///T Specific Parent
			/// Gets the parent symbol if the parent is of a specific type
			///M
			template <class T>
			T* Parent() const;
			///M

			///T Name
			/// Gets the name of the symbol
			virtual Scope Name();

			///T Absolute Name
			/// Gets the absolute name of the symbol
			virtual ScopeList AbsoluteName();

			///T Type
			/// Gets the type of the symbol
			///R Symbol*: The symbol of the type or <code>nullptr</code> if the symbol does not have a type
			virtual TypeSymbol* Type();

			///T Specialize Template
			/// Specializes a template symbol
			virtual Symbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) = 0;

			///T Find
			/// Finds a specific symbol
			/// Returns <code>nullptr</code> if a symbol was not found
			///M
			Symbol* Find(const Scope& scope, const FileInfo& file);
			Symbol* Find(const ScopeList& scopeList, const FileInfo& file);
			///M

			///T Find Specific type
			/// Finds a specific symbol of a specific symbol type specified by the template argument
			///M
			template <class T>
			T* Find(const Scope& scope, const FileInfo& file);
			template <class T>
			T* Find(const ScopeList& scope, const FileInfo& file);
			///M

			///T Contains
			/// Checks if the specified symbol exists and returns it
			/// If the symbol was not found <code>nullptr</code> is returned
			///M
			Symbol* Contains(const Scope& scope);
			Symbol* Contains(const ScopeList& scopeList);
			///M

			///T Contains Specific type
			/// Checks if the specified symbol of the specified type exists and returns it
			/// If the symbol was not found <code>nullptr</code> is returned
			///M
			template <class T>
			T* Contains(const Scope& scope);
			template <class T>
			T* Contains(const ScopeList& scope);
			///M

			///T Is
			/// Checks if the symbol is a specific type of symbol
			///M
			template <class T>
			bool Is() {
			///M
				return dynamic_cast<T*>(this) != nullptr;
			}

			///T Cast
			/// Casts the symbol to a specific symbol type
			/// Returns <code>nullptr</code> if the cast fails
			///M
			template <class T>
			T* Cast() {
			///M
				return dynamic_cast<T*>(this);
			}

		protected:
			virtual Symbol* FindSymbol(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			void FindError(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			FileInfo file;
			Symbol* parent = nullptr;
			Scope name;

			friend MapSymbol;
			friend SymbolTable;
		};

		template <class T>
		inline T* Symbol::Parent() const {
			return parent->Cast<T>();
		}

		template <class T>
		inline T* Symbol::Find(const Scope& scope, const FileInfo& file) {
			if (Symbol* const sym = Find(ScopeList().Add(scope), file)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Find(const ScopeList& scopeList, const FileInfo& file) {
			if (Symbol* const sym = Find(scopeList, file)) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Contains(const Scope& scope) {
			if (Symbol* const sym = Contains(ScopeList().Add(scope))) {
				return sym->Cast<T>();
			}
			else {
				return nullptr;
			}
		}

		template <class T>
		inline T* Symbol::Contains(const ScopeList& scopeList) {
			ErrorLog::AddMarker();
			Symbol* const sym = FindSymbol(scopeList, 0, FileInfo());
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
