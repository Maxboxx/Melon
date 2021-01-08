#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class FunctionSymbol;

		///B TypeSymbol
		/// A symbol for a type
		class TypeSymbol : public MapSymbol {
		public:
			TypeSymbol(const Boxx::UInt size, const FileInfo& file);
			~TypeSymbol();

			///T Size
			/// Gets the stack size of the type
			virtual Boxx::UInt Size() const;

			///T Memory Size
			/// Gets the memory size of the type
			virtual Boxx::UInt MemorySize() const;

			///T Implicit Conversion To
			/// Checks if the type can be implicitly converted to the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionTo(TypeSymbol* const type);

			///T Explicit Conversion To
			/// Checks if the type can be explicitly converted to the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionTo(TypeSymbol* const type);

			///T Implicit Conversion From
			/// Checks if the type can be implicitly converted from the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionFrom(TypeSymbol* const type);

			///T Explicit Conversion From
			/// Checks if the type can be explicitly converted from the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionFrom(TypeSymbol* const type);

			virtual TypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			Boxx::UInt size;
		};
	}
}
