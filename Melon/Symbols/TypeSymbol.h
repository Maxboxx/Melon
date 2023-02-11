#pragma once

#include "Boxx/Set.h"
#include "Boxx/Map.h"

#include "MapSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		class FunctionSymbol;
		class TemplateSymbol;
		class TemplateTypeSymbol;

		/// A symbol for a type.
		class TypeSymbol : public MapSymbol {
		public:
			TypeSymbol(const Boxx::UInt size, const FileInfo& file);
			~TypeSymbol();

			/// Gets the stack size of the type.
			virtual Boxx::UInt Size() const;

			/// Gets the memory size of the type.
			virtual Boxx::UInt MemorySize() const;

			/// Checks if the type can be implicitly converted to the specified type.
			///R The conversion operator for the type.
			/// {nullptr} if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionTo(TypeSymbol* const type);

			/// Checks if the type can be explicitly converted to the specified type.
			///R The conversion operator for the type.
			/// {nullptr} if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionTo(TypeSymbol* const type);

			/// Checks if the type can be implicitly converted from the specified type.
			///R The conversion operator for the type.
			/// {nullptr} if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionFrom(TypeSymbol* const type);

			/// Checks if the type can be explicitly converted from the specified type.
			///R The conversion operator for the type.
			/// {nullptr} if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionFrom(TypeSymbol* const type);

			/// Finds a method of the current type.
			FunctionSymbol* FindMethod(const Symbols::Name& name, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds a method of the current type.
			FunctionSymbol* FindMethod(const Symbols::Name& name, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds a unary operator of the current type.
			FunctionSymbol* FindUnaryOperator(const Symbols::Name& name, const FileInfo& file);

			/// Checks if the type is or inherits from the specified type.
			virtual bool IsOfType(TypeSymbol* const type);

			/// Checks if the type has been initialized.
			virtual bool IsInitialized();

			/// Prepares the initialization for the scan step.
			virtual void PrepareInit();

			/// Completes the initialization for the scan step.
			virtual void CompleteInit();

			/// Get all unassigned members.
			virtual Boxx::Set<Symbols::Name> UnassignedMembers();

			/// Calculates the size of the type and updates the internal value.
			virtual void UpdateSize();

			/// Uses the specified type to deduce the template values of the current type.
			///R A map containing the deduced template types.
			Boxx::Map<TemplateSymbol*, TypeSymbol*> DeduceTemplates(TypeSymbol* const type);

			/// Gets the kiwi type of symbol.
			virtual Kiwi::Type KiwiType();
			
			virtual TypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			friend TemplateSymbol;

			static void DeduceTemplates(TemplateTypeSymbol* const type1, TemplateTypeSymbol* const type2, Boxx::Map<TemplateSymbol*, TypeSymbol*>& templateMap);

			Boxx::UInt size;
		};
	}
}
