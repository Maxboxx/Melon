#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class ScopeList;

		class Symbol;

		///B ScopeSymbol
		/// A symbol for a scope
		class ScopeSymbol : public MapSymbol {
		public:
			ScopeSymbol(const FileInfo& file);
			~ScopeSymbol();

			///T Add Scope
			/// Adds a new sub scope to the table
			///R ScopeSymbol*: The table for the new scope
			ScopeSymbol* AddScope(const FileInfo& file);

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

		private:
			Boxx::List<ScopeSymbol*> scopes;

			static ScopeSymbol globalTable;
		};
	}
}
