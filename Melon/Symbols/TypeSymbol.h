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

			///T Size
			/// Gets the stack size of the type
			virtual Boxx::UInt Size() const;

			///T Memory Size
			/// Gets the memory size of the type
			virtual Boxx::UInt MemorySize() const;

			virtual TypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			Boxx::UInt size;
		};
	}
}
