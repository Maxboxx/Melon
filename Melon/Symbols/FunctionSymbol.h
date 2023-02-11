#pragma once

#include "Boxx/Pointer.h"
#include "Boxx/Optional.h"

#include "ScopeSymbol.h"

#include "Nodes/SymbolNode.h"

///N Melon::Symbols
namespace Melon {
	namespace Nodes {
		class Node;
		class StructStatement;
	}

	namespace Symbols {
		/// Bit flags for function modifiers.
		enum class FunctionModifiers : Boxx::UByte {
			None = 0,
			Static = 1,
			Override = 2,
			Required = 4,
			Partial = 8
		};

		BOXX_ENUM_FLAGS(FunctionModifiers);

		/// Bit flags for function attributes.
		enum class FunctionAttributes : Boxx::UByte {
			None = 0,
			Get = 1,
			Ref = 2,
			Set = 4,
			Throw = 8
		};

		BOXX_ENUM_FLAGS(FunctionAttributes);

		class VariableSymbol;

		/// A symbol for functions.
		class FunctionSymbol : public ScopeSymbol {
		public:
			FunctionSymbol(const FileInfo& file);
			~FunctionSymbol();

			/// Get the return type at the specified index.
			TypeSymbol* ReturnType(const Boxx::UInt index);

			/// Get the argument type at the specified index.
			TypeSymbol* ArgumentType(const Boxx::UInt index);

			/// Get the argument at the specified index.
			VariableSymbol* Argument(const Boxx::UInt index);

			/// Get the template argument at the specified index.
			TypeSymbol* TemplateArgument(const Boxx::UInt index);

			/// The number of required arguments.
			Boxx::UInt RequiredArguments() const;

			/// The number of required template arguments.
			Boxx::UInt RequiredTemplateArguments() const;

			/// Adds an overload.
			FunctionSymbol* AddOverload(FunctionSymbol* const overload);

			/// Finds the best match for a function overload.
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds the best match for a function overload.
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds the best match for a static function overload.
			FunctionSymbol* FindStaticOverload(const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds the best match for a static function overload.
			FunctionSymbol* FindStaticOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds the best match for a method overload.
			FunctionSymbol* FindMethodOverload(const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// Finds the best match for a method overload.
			FunctionSymbol* FindMethodOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);

			/// The template arguments of the function.
			Boxx::List<NameList> templateArguments;

			/// The relative return types of the function.
			Boxx::List<NameList> returnValues;

			/// The names of the function arguments.
			Boxx::List<NameList> arguments;

			/// The node to use for the compilation of the function.
			///p Is {nullptr} if {node} should be used instead.
			Boxx::Pointer<Nodes::SymbolNode> symbolNode;

			/// The node to use for the compilation of the function.
			///p Is {nullptr} if {symbolNode} should be used instead.
			Weak<Melon::Nodes::FunctionBody> node;

			/// The function modifiers.
			FunctionModifiers modifiers = FunctionModifiers::None;

			/// The function attributes.
			FunctionAttributes attributes = FunctionAttributes::None;

			/// {true} if the type conversion is explicit.
			///p Only used if the function is a type conversion function.
			bool isExplicit = false;

			/// {true} if the function is an operator function.
			bool isOperator = false;

			virtual bool IsNotSpecialized() override;
			virtual void SetTemplateValues(Symbol* const symbol) override;
			virtual FunctionSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

			FunctionSymbol* replace = nullptr;

		protected:
			virtual Symbol* FindSymbol(const NameList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

			Boxx::Tuple<Boxx::List<TypeSymbol*>, Boxx::List<NameList>> FindTemplateArguments(FunctionSymbol* const func, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			
			FunctionSymbol* FindOverload(const Boxx::List<FunctionSymbol*>& overloads, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file);

		private:
			friend TypeSymbol;
			friend SymbolTable;
			friend Melon::Nodes::StructStatement;
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
