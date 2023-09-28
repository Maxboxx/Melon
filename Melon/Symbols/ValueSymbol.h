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

			/// The type of the enum value.
			Boxx::Optional<NameList> type;

			ValueSymbol(const FileInfo& file);
			~ValueSymbol();

			/// The type of the value data.
			TypeSymbol* ValueType();

			virtual TypeSymbol* Type() override;
			virtual ValueSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
