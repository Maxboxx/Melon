#pragma once

#include "Symbol.h"

namespace Melon {
	namespace Symbols {
		///B ValueSymbol
		/// Contains information about an enum value
		class ValueSymbol : public Symbol {
		public:
			Boxx::Long value = 0;

			ValueSymbol(const FileInfo& file);
			~ValueSymbol();

			virtual Symbol* Type();
		};
	}
}
