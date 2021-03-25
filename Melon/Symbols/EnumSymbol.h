#pragma once

#include "IntegerSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Contains information about an enum.
		class EnumSymbol : public IntegerSymbol {
		public:
			/// A list of all values.
			Boxx::List<Scope> values;

			EnumSymbol(const Boxx::UInt size, const bool isSigned, const FileInfo& file);
			~EnumSymbol();

			virtual EnumSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
