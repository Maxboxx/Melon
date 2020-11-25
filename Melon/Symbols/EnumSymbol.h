#pragma once

#include "SymbolTable.h"

namespace Melon {
	namespace Symbols {
		///B EnumSymbol
		/// Contains information about an enum
		class EnumSymbol : public MapSymbol {
		public:
			Boxx::UByte size = 1;
			bool isSigned = false;

			EnumSymbol(const FileInfo& file);
			~EnumSymbol();

			virtual Symbol* Type();
			virtual bool IsType() const;
			virtual Boxx::UInt Size() const;
		};
	}
}
