#pragma once

#include "TypeSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// A symbol for an integer type.
		class IntegerSymbol : public TypeSymbol {
		public:
			IntegerSymbol(const Boxx::UInt size, const bool isSigned, const FileInfo& file);
			~IntegerSymbol();

			/// {true} if the integer is signed.
			bool IsSigned() const;

			virtual Boxx::String KiwiName() override;

			virtual IntegerSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			bool isSigned;
		};
	}
}
