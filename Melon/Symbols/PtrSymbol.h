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

			virtual Kiwi::Type KiwiType() override;

			virtual TypeSymbol* InitializeSpecialize() override;
		};
	}
}
