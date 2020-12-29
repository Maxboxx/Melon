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

			///T Add Overload
			/// Adds an overload
			FunctionSymbol* AddOverload(FunctionSymbol* const overload);

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
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

		private:
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
