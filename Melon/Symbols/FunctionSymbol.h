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

			Boxx::List<ScopeList> templateArguments;
			Boxx::List<ScopeList> returnValues;
			Boxx::List<Scope>     arguments;

			Boxx::Pointer<Nodes::SymbolNode> symbolNode;
			Boxx::Pointer<Node> node;

			FunctionAttributes attributes = FunctionAttributes::None;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

		private:
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
