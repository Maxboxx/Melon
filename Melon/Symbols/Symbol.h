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
			virtual Scope Name();

			/// Gets the absolute name of the symbol.
			virtual ScopeList AbsoluteName();

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
			Symbol* Find(const Scope& scope, const FileInfo& file);

			/// Finds a specific symbol in the current symbol.
			Symbol* Find(const ScopeList& scopeList, const FileInfo& file);

			/// Finds a specific symbol of the specified type.
			///M
			template <class T>
			T* Find(const Scope& scope, const FileInfo& file);
			///M

			/// Finds a specific symbol of the specified type.
			///M
			template <class T>
			T* Find(const ScopeList& scope, const FileInfo& file);
			///M

			/// Checks if the specified symbol exists and returns it.
			///p If the symbol was not found {nullptr} is returned.
			Symbol* Contains(const Scope& scope);

			/// Checks if the specified symbol exists and returns it.
			///p If the symbol was not found {nullptr} is returned.
			Symbol* Contains(const ScopeList& scopeList);

			/// Checks if a specific symbol of the specified type exists and returns it.
			///p If the symbol was not found {nullptr} is returned
			///M
			template <class T>
			T* Contains(const Scope& scope);
			///M

			/// Checks if a specific symbol of the specified type exists and returns it.
			///p If the symbol was not found {nullptr} is returned
			///M
			template <class T>
			T* Contains(const ScopeList& scope);
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

		protected:
			static TypeSymbol* ReplaceType(TypeSymbol* const type, const Boxx::ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file);
			static ScopeList ReplaceTypeScope(TypeSymbol* const type, const Boxx::ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file);

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
