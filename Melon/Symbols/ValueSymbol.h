#pragma once

#include "Symbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Contains information about an enum value.
		class ValueSymbol : public Symbol {
		public:
			/// The numerical value of the enum value.
			Boxx::Long value = 0;

			ValueSymbol(const FileInfo& file);
			~ValueSymbol();

			virtual TypeSymbol* Type() override;
			virtual ValueSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
