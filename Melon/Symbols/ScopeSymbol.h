#pragma once

#include "MapSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		class ScopeList;

		class Symbol;

		/// A symbol for a scope.
		class ScopeSymbol : public MapSymbol {
		public:
			ScopeSymbol(const FileInfo& file);
			~ScopeSymbol();

			/// Adds a new child scope to the scope.
			///R The new scope.
			ScopeSymbol* AddScope(const FileInfo& file);

			virtual ScopeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			virtual Symbol* FindSymbol(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			void AddScope(ScopeSymbol* const scope);

			Boxx::List<ScopeSymbol*> scopes;
		};
	}
}
