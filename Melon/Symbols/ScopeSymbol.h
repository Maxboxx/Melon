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

			virtual ScopeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			void AddScope(ScopeSymbol* const scope);

			Boxx::List<ScopeSymbol*> scopes;
		};
	}
}
