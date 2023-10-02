#pragma once

#include "TypeSymbol.h"

namespace Melon {
	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a pointer.
		class PtrSymbol : public TypeSymbol {
		public:
			NameList type;

			PtrSymbol(const FileInfo& file);
			~PtrSymbol();

			/// The pointer type of the symbol.
			TypeSymbol* PtrType();

			virtual Kiwi::Type KiwiType() override;

			virtual TypeSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
