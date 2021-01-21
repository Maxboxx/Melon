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
			
			virtual StructSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
			
			virtual bool IsInitialized() override;
			virtual void PrepareInit() override;
			virtual void CompleteInit() override;

			///T Non static members
			Boxx::Collection<Scope> members;
		};
	}
}
