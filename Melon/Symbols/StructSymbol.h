#pragma once

#include "TemplateTypeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B StructSymbol
		/// Contains information about a struct
		class StructSymbol : public TemplateTypeSymbol {
		public:
			StructSymbol(const FileInfo& file);
			~StructSymbol();

			///T Non static members
			Boxx::Collection<Scope> members;
		};
	}
}
