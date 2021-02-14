#pragma once

#include "TemplateTypeSymbol.h"

namespace Melon {
	namespace Nodes {
		class Node;
	}

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
			virtual Boxx::Set<Scope> UnassignedMembers() override;
			virtual void UpdateSize() override;

			///T Non static members
			Boxx::List<Scope> members;

			///T Is Recursive
			bool isRecursive = false;

			///T Node
			Boxx::Pointer<Melon::Nodes::Node> node = nullptr;
		};
	}
}
