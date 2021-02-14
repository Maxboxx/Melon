#pragma once

#include "TypeSymbol.h"

namespace Melon {
	namespace Nodes {
		class RootNode;
		class StructNode;
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
			TypeSymbol* TemplateArgument(const Boxx::UInt index);

			///T Can Be Deduced
			/// Checks if template arguments of the current type can be deduced by the specified type
			bool CanBeDeduced(TemplateTypeSymbol* const type);

			Boxx::List<ScopeList> templateArguments;

			virtual Scope Name() override;
			virtual bool IsNotSpecialized() override;
			virtual void SetTemplateValues(Symbol* const symbol) override;
			virtual TemplateTypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			virtual Symbol* FindSymbol(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file);

			friend Melon::Nodes::RootNode;
			friend Melon::Nodes::StructNode;

			Boxx::List<TemplateTypeSymbol*> templateVariants;

			TemplateTypeSymbol* templateParent = nullptr;
		};
	}
}
