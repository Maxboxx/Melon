#pragma once

#include "TypeSymbol.h"

namespace Melon {
	namespace Nodes {
		class RootNode;
	}

	namespace Symbols {
		///B TemplateTypeSymbol
		/// A symbol for a template type
		class TemplateTypeSymbol : public TypeSymbol {
		public:
			TemplateTypeSymbol(const FileInfo& file);
			~TemplateTypeSymbol();

			///T Add Template Variant
			/// Adds a template variant to the symbol
			void AddTemplateVariant(TemplateTypeSymbol* const sym);

			///T Template Argument
			/// Get the template argument at the specified index
			TypeSymbol* TemplateArgument(const Boxx::UInt index) const;

			Boxx::List<ScopeList> templateArguments;

			virtual TemplateTypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			friend Melon::Nodes::RootNode;

			Boxx::List<TemplateTypeSymbol*> templateVariants;
		};
	}
}
