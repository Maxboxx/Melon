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

		class FunctionSymbol : public ScopeSymbol {
		public:
			FunctionSymbol(const FileInfo& file);
			~FunctionSymbol();

			///T Return Type
			/// Get the return type at the specified index
			Symbol* ReturnType(const Boxx::UInt index) const;

			///T Argument
			/// Get the argument at the specified index
			Symbol* Argument(const Boxx::UInt index) const;

			///T Template Argument
			/// Get the template argument at the specified index
			Symbol* TemplateArgument(const Boxx::UInt index) const;

			///T Add Overload
			/// Adds an overload
			FunctionSymbol* AddOverload(FunctionSymbol* const overload);

			///T Template arguments
			Boxx::List<ScopeList> templateArguments;

			///T Return values
			Boxx::List<ScopeList> returnValues;

			///T Arguments
			/// Argument names if the function uses a <code>Node</code>
			/// Argument types if the function uses a <code>SymbolNode</code>
			Boxx::List<ScopeList> arguments;

			///T Node
			/// The node to use for the compilation of the function
			///M
			Boxx::Pointer<Nodes::SymbolNode> symbolNode;
			Boxx::Pointer<Melon::Nodes::Node> node;
			///M

			///T Function attrubutes
			FunctionAttributes attributes = FunctionAttributes::None;

			virtual Scope Name() const override;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

		private:
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
