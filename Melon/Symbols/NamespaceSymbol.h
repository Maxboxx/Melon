#pragma once

#include "ScopeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B NamespaceSymbol
		/// A symbol for a namespace
		class NamespaceSymbol : public ScopeSymbol {
		public:
			NamespaceSymbol(const Boxx::String& includedPath, const FileInfo& file);
			~NamespaceSymbol();

			Boxx::String IncludedPath() const;

		protected:
			Boxx::String includedPath;
		};
	}
}
