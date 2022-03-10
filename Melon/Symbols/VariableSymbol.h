#pragma once

#include "Symbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Bit flags for the different attributes a variable can have.
		enum class VariableAttributes : Boxx::UByte {
			/// No attribute.
			None = 0,

			/// The {const} attribute.
			Const = 1,

			/// The {ref} attribute.
			Ref = 2,

			/// The {static} attribute.
			Static = 4
		};

		BOXX_ENUM_FLAGS(VariableAttributes);

		/// Contains information about a variable.
		class VariableSymbol : public Symbol {
		public:
			/// The relative type of the variable.
			NameList type;

			/// The attributes of the variable.
			VariableAttributes attributes = VariableAttributes::None;

			/// {true} if the variable has been assigned.
			bool isAssigned = false;

			/// Stack offset for member variables.
			Boxx::UInt stackIndex = 0;

			VariableSymbol(const FileInfo& file);
			~VariableSymbol();

			/// Checks if the variable has the spceified attribute.
			bool HasAttribute(const VariableAttributes attribute);

			/// Gets the memory size of the variable.
			Boxx::UInt Size();

			virtual TypeSymbol* Type() override;
			virtual VariableSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
