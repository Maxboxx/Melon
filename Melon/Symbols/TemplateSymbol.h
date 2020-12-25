#pragma once

#include "Symbol.h"

namespace Melon {
	namespace Symbols {
		///B TemplateSymbol
		/// Contains information about a template symbol
		class TemplateSymbol : public Symbol {
		public:
			///T Type
			/// The absolute type of the template symbol
			ScopeList type;

			TemplateSymbol(const FileInfo& file);
			~TemplateSymbol();

			virtual TypeSymbol* Type() override;
		};
	}
}
