#pragma once

#include "Boxx/Set.h"

namespace Melon {
	namespace Symbols {
		class VariableSymbol;
		class TypeSymbol;
		class FunctionSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		///H Structs

		/// Info about optimization.
		struct OptimizeInfo {
			/// Has the code been optimized.
			bool optimized = false;

			/// All used variables.
			Boxx::Set<Symbols::VariableSymbol*> usedVariables;

			/// All used types.
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;

			/// All used functions.
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;
		};
	}
}
