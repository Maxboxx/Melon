#pragma once

#include "Boxx/Pointer.h"

#include "ScopeSymbol.h"

#include "Nodes/SymbolNode.h"

namespace Melon {
	namespace Nodes {
		class Node;
	}

	namespace Symbols {
		enum class FunctionAttributes : Boxx::UByte {
			None = 0,
			Static = 1,
			Override = 2,
			Required = 4,
			Debug = 8,
			Partial = 16
		};

		BOXX_ENUM_FLAGS(FunctionAttributes);

		class VariableSymbol;

		///B FunctionSymbol
		/// A symbol for functions
		class FunctionSymbol : public ScopeSymbol {
		public:
			FunctionSymbol(const FileInfo& file);
			~FunctionSymbol();

			///T Return Type
			/// Get the return type at the specified index
			TypeSymbol* ReturnType(const Boxx::UInt index);

			///T Argument Type
			/// Get the argument type at the specified index
			TypeSymbol* ArgumentType(const Boxx::UInt index);

			///T Argument
			/// Get the argument at the specified index
			VariableSymbol* Argument(const Boxx::UInt index);

			///T Template Argument
			/// Get the template argument at the specified index
			TypeSymbol* TemplateArgument(const Boxx::UInt index);

			///T Number of Required Arguments
			Boxx::UInt RequiredArguments() const;

			///T Number of Required Template Arguments
			Boxx::UInt RequiredTemplateArguments() const;

			///T Add Overload
			/// Adds an overload
			FunctionSymbol* AddOverload(FunctionSymbol* const overload);

			///T Find Overload
			/// Finds the best match for a function overload
			///M
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			///M

			///T Find Method Overload
			/// Finds the best match for a method overload
			///M
			FunctionSymbol* FindMethodOverload(const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindMethodOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			///M

			///T Template arguments
			Boxx::List<ScopeList> templateArguments;

			///T Return values
			Boxx::List<ScopeList> returnValues;

			///T Arguments
			/// Argument names
			Boxx::List<ScopeList> arguments;

			///T Node
			/// The node to use for the compilation of the function
			///M
			Boxx::Pointer<Nodes::SymbolNode> symbolNode;
			Boxx::Pointer<Melon::Nodes::Node> node;
			///M

			///T Function attrubutes
			FunctionAttributes attributes = FunctionAttributes::None;

			///T Is Explicit
			/// Used for type conversions
			bool isExplicit = false;

			virtual FunctionSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

		protected:
			virtual Symbol* FindSymbol(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

			Boxx::Tuple<Boxx::List<TypeSymbol*>, Boxx::List<TypeSymbol*>> FindTemplateArgument(FunctionSymbol* const func, TypeSymbol* const templateArg, TypeSymbol* const arg, const FileInfo& file);
			Boxx::Tuple<Boxx::List<TypeSymbol*>, Boxx::List<TypeSymbol*>> FindTemplateArguments(FunctionSymbol* const func, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<FunctionSymbol*>& overloads, const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& args, const bool isStatic, const FileInfo& file);
			FunctionSymbol* FindOverload(const Boxx::List<TypeSymbol*>& templateArgs, const Boxx::List<TypeSymbol*>& args, const bool isStatic, const FileInfo& file);

		private:
			friend TypeSymbol;
			friend SymbolTable;
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
