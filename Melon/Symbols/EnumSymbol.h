#pragma once

#include "IntegerSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Contains information about an enum.
		class EnumSymbol : public IntegerSymbol {
		public:
			/// A list of names for all values.
			Boxx::List<Symbols::Name> values;

			EnumSymbol(const Boxx::UInt size, const bool isSigned, const FileInfo& file);
			~EnumSymbol();

			virtual EnumSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
