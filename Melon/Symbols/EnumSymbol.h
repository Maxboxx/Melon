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

			virtual EnumSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

			///T Values
			Boxx::Collection<Scope> values;
		};
	}
}
