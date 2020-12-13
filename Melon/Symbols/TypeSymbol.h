#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		///B TypeSymbol
		/// A symbol for a type
		class TypeSymbol : public MapSymbol {
		public:
			TypeSymbol(const Boxx::UInt size, const FileInfo& file);
			~TypeSymbol();

			Boxx::UInt Size() const;

		protected:
			Boxx::UInt size;
		};
	}
}
