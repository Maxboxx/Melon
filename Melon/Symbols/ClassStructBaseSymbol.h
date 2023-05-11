#pragma once

#include "TemplateTypeSymbol.h"

namespace Melon {
	namespace Nodes {
		class Node;
		class ClassStatement;
	}

	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a class or struct.
		class ClassStructBaseSymbol : public TemplateTypeSymbol {
		public:
			ClassStructBaseSymbol(const FileInfo& file);
			~ClassStructBaseSymbol();
			
			virtual ClassStructBaseSymbol* InitializeSpecialize() override = 0;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
			
			virtual bool IsInitialized() override;
			virtual void PrepareInit() override;
			virtual void CompleteInit() override;
			virtual Boxx::Set<Symbols::Name> UnassignedMembers() override;
			virtual void UpdateSize() override;

			/// All non static members.
			Boxx::List<Symbols::Name> members;
		};
	}
}
