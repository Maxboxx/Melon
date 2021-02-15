#pragma once

#include "Boxx/Set.h"

namespace Melon {
	namespace Symbols {
		class VariableSymbol;
		class TypeSymbol;
		class FunctionSymbol;
	}

	namespace Nodes {
		///B OptimizeInfo
		/// Info about optimization
		struct OptimizeInfo {
			///T Optimized
			/// Whether or not the code has been optimized
			bool optimized = false;

			///T Used Variables
			Boxx::Set<Symbols::VariableSymbol*> usedVariables;

			///T Used Types
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;

			///T Used functions
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;
		};
	}
}
