#pragma once

#include "Boxx/Set.h"
#include "Boxx/Map.h"

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class FunctionSymbol;
		class TemplateSymbol;
		class TemplateTypeSymbol;

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

			///T Implicit Conversion To
			/// Checks if the type can be implicitly converted to the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionTo(TypeSymbol* const type);

			///T Explicit Conversion To
			/// Checks if the type can be explicitly converted to the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionTo(TypeSymbol* const type);

			///T Implicit Conversion From
			/// Checks if the type can be implicitly converted from the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ImplicitConversionFrom(TypeSymbol* const type);

			///T Explicit Conversion From
			/// Checks if the type can be explicitly converted from the specified type.
			///R FunctionSymbol*: The conversion operator for the type.
			/// nullptr if a conversion operator was not found.
			FunctionSymbol* ExplicitConversionFrom(TypeSymbol* const type);

			///T Find Method
			/// Finds a method of the current type
			///M
			FunctionSymbol* FindMethod(const Scope& name, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindMethod(const Scope& name, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			///M

			///T Find Unary Operator
			/// Finds a unary operator of the current type
			FunctionSymbol* FindUnaryOperator(const Scope& name, const FileInfo& file);

			///T Is Of Type
			/// Checks if the type is or inherits from the specified type
			virtual bool IsOfType(TypeSymbol* const type);

			///T Is Initialized
			/// Checks if the type has been initialized
			virtual bool IsInitialized();

			///T Prepare Initialization
			/// Prepares the initialization for the scan step
			virtual void PrepareInit();

			///T Complete Initialization
			/// Completes the initialization for the scan step
			virtual void CompleteInit();

			///T Unassigned Members
			/// Get all unassigned members
			virtual Boxx::Set<Scope> UnassignedMembers();

			///T Update Size
			/// Calculates the size of the type and updates the internal value
			virtual void UpdateSize();

			///T Deduce Templates
			/// Uses the specified type to deduce the template values of the current type
			///R Map<TemplateSymbol*, TypeSymbol*>: A map containing the deduced template types.
			Boxx::Map<TemplateSymbol*, TypeSymbol*> DeduceTemplates(TypeSymbol* const type);
			
			virtual TypeSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override = 0;

		protected:
			friend TemplateSymbol;

			static void DeduceTemplates(TemplateTypeSymbol* const type1, TemplateTypeSymbol* const type2, Boxx::Map<TemplateSymbol*, TypeSymbol*>& templateMap);

			Boxx::UInt size;
		};
	}
}
