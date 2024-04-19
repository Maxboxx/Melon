#pragma once

#include "Symbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		/// Bit flags for the different modifiers a variable can have.
		enum class VariableModifiers : Boxx::UByte {
			/// No attribute.
			None = 0,

			/// The {const} modifier.
			Const = 1,

			/// The {ref} modifier.
			Ref = 2,

			/// The {static} modifier.
			Static = 4
		};

		BOXX_ENUM_FLAGS(VariableModifiers);

		/// Contains information about a variable.
		class VariableSymbol : public Symbol {
		public:
			/// The relative type of the variable.
			NameList type;

			/// The modifiers of the variable.
			VariableModifiers modifiers = VariableModifiers::None;

			/// {true} if the variable has been assigned.
			bool isAssigned = false;

			/// Stack offset for member variables.
			Boxx::UInt stackIndex = 0;

			VariableSymbol(const FileInfo& file);
			~VariableSymbol();

			/// Checks if the variable has the spceified attribute.
			bool HasAttribute(const VariableModifiers attribute);

			/// Gets the memory size of the variable.
			Boxx::UInt Size();

			virtual Kiwi::Type KiwiType() override;
			virtual TypeSymbol* Type() override;
			virtual VariableSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;
		};
	}
}
