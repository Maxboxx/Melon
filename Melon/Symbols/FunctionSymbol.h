#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class FunctionSymbol : public MapSymbol {
		public:
			Boxx::List<ScopeList> returnValues;
			Boxx::List<Boxx::String> arguments;

			FunctionSymbol(const FileInfo& file);
			~FunctionSymbol();

			///T Return Type
			/// Get the return type at the specified index
			Symbol* ReturnType(const Boxx::UInt index) const;

			///T Argument
			/// Get the argument at the specified index
			Symbol* Argument(const Boxx::UInt index) const;
		};
	}
}
