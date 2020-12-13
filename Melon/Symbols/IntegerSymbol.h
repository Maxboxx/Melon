#pragma once

#include "TypeSymbol.h"

namespace Melon {
	namespace Symbols {
		///B IntegerSymbol
		/// A symbol for an integer type
		class IntegerSymbol : public TypeSymbol {
		public:
			IntegerSymbol(const Boxx::UInt size, const bool isSigned, const FileInfo& file);
			~IntegerSymbol();

			bool IsSigned() const;

		private:
			bool isSigned;
		};
	}
}
