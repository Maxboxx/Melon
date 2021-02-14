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

			///T Is Signed
			/// Checks if the integer is signed or not
			bool IsSigned() const;

			virtual IntegerSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			bool isSigned;
		};
	}
}
