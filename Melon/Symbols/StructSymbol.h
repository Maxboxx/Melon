#pragma once

#include "TemplateTypeSymbol.h"

namespace Melon {
	namespace Nodes {
		class Node;
	}

	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a struct.
		class StructSymbol : public TemplateTypeSymbol {
		public:
			StructSymbol(const FileInfo& file);
			~StructSymbol();
			
			virtual StructSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
			
			virtual bool IsInitialized() override;
			virtual void PrepareInit() override;
			virtual void CompleteInit() override;
			virtual Boxx::Set<Symbols::Name> UnassignedMembers() override;
			virtual void UpdateSize() override;

			/// All non static members.
			Boxx::List<Symbols::Name> members;

			/// {true} if the scope is recursive.
			bool isRecursive = false;

			/// The struct node for this symbol.
			Weak<Melon::Nodes::StructStatement> node;
		};
	}
}
