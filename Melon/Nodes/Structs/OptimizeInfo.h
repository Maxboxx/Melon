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
		struct OptimizeInfo {
			bool optimized = false;

			Boxx::Set<Symbols::VariableSymbol*> usedVariables;
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;
		};
	}
}
