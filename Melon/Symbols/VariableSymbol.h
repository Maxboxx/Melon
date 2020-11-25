#pragma once

#include "Symbol.h"

namespace Melon {
	namespace Symbols {

		///B VariableAttributes
		/// The different attributes a variable can have
		enum class VariableAttributes : Boxx::UByte {
			None  = 0,
			Const = 1,
			Ref   = 2
		};

		BOXX_ENUM_FLAGS(VariableAttributes);

		///B VariableSymbol
		/// Contains information about a variable
		class VariableSymbol : public Symbol {
		public:
			///T Type
			/// The relative type of the variable
			ScopeList type;

			///T Attributes
			VariableAttributes attributes = VariableAttributes::None;

			VariableSymbol(const FileInfo& file);
			~VariableSymbol();

			virtual Symbol* Type() override;
		};
	}
}
