#pragma once

#include "TemplateTypeSymbol.h"

namespace Melon {
	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a pointer.
		class PtrSymbol : public TemplateTypeSymbol {
		public:
			NameList type;

			PtrSymbol(const FileInfo& file);
			~PtrSymbol();

			/// The pointer type of the symbol.
			TypeSymbol* PtrType();

			virtual Kiwi::Type KiwiType() override;

			virtual Boxx::UInt Size() const override;
			virtual void UpdateSize() override;

			virtual PtrSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
