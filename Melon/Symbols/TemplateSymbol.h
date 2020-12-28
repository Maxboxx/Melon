#pragma once

#include "TypeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B TemplateSymbol
		/// Contains information about a template symbol
		class TemplateSymbol : public TypeSymbol {
		public:
			///T Type
			/// The absolute type of the template symbol
			ScopeList type;

			TemplateSymbol(const FileInfo& file);
			~TemplateSymbol();

			virtual TypeSymbol* Type() override;
			virtual TemplateSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
