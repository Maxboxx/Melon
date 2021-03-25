#pragma once

#include "ScopeSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// A symbol for a namespace.
		class NamespaceSymbol : public ScopeSymbol {
		public:
			NamespaceSymbol(const Boxx::String& includedPath, const FileInfo& file);
			~NamespaceSymbol();

			/// The include path for this namespace.
			Boxx::String IncludedPath() const;

			virtual NamespaceSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			Boxx::String includedPath;
		};
	}
}
