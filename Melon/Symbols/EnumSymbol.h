#pragma once

#include "IntegerSymbol.h"

namespace Melon {
	namespace Symbols {
		///B EnumSymbol
		/// Contains information about an enum
		class EnumSymbol : public IntegerSymbol {
		public:
			EnumSymbol(const Boxx::UInt size, const bool isSigned, const FileInfo& file);
			~EnumSymbol();

			Boxx::Collection<Scope> values;
		};
	}
}
