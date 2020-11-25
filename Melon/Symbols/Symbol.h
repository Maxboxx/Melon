#pragma once

#include "Boxx/String.h"
#include "Boxx/Set.h"
#include "Boxx/List.h"
#include "Boxx/Map.h"

#include "ScopeList.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Symbols {
		class MapSymbol;

		///B Symbol
		/// Contains information about a symbol
		class Symbol {
		public:
			Symbol(const FileInfo& file);
			~Symbol();

			///T Parent
			/// Gets the parent symbol
			Symbol* Parent() const;

			///T Name
			/// Gets the name of the symbol
			virtual Scope Name() const;

			///T Absolute Name
			/// Gets the absolute name of the symbol
			virtual ScopeList AbsoluteName() const;

			///T Type
			/// Gets the type of the symbol
			virtual Symbol* Type();

			///T Is Type
			/// Checks if the symbol is a type
			virtual bool IsType() const;

			///T Size
			/// Gets the size of the symbol
			virtual Boxx::UInt Size() const;

			///T Is
			/// Checks if the symbol is a specific type of symbol
			///M
			template <class T>
			bool Is() const {
			///M
				return dynamic_cast<T*>(this) != nullptr;
			}

			///T Cast
			/// Casts the symbol to a specific symbol type
			/// Returns <code>nullptr</code> if the cast fails
			///M
			template <class T>
			T* Cast() const {
			///M
				return dynamic_cast<T*>(this);
			}

		protected:
			FileInfo file;
			Symbol* parent = nullptr;
			Boxx::String name;

			friend MapSymbol;
		};
	}
}
