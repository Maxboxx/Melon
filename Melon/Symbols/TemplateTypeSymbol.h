#pragma once

#include "TypeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B TemplateTypeSymbol
		/// A symbol for a template type
		class TemplateTypeSymbol : public TypeSymbol {
		public:
			TemplateTypeSymbol(const FileInfo& file);
			~TemplateTypeSymbol();

			Boxx::List<ScopeList> templateArguments;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

		private:
			Boxx::List<TemplateTypeSymbol*> templateVariants;
		};
	}
}
