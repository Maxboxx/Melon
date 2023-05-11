#pragma once

#include "TypeSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Contains information about a template symbol.
		class TemplateSymbol : public TypeSymbol {
		public:
			/// The absolute type of the template symbol.
			NameList type;

			TemplateSymbol(const FileInfo& file);
			~TemplateSymbol();

			virtual TypeSymbol* Type() override;
			virtual TemplateSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
