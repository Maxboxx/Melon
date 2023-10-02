#pragma once

#include "ScopeInfo.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Symbols {
		class TypeSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		///H Structs

		/// Info used for scanning.
		struct ScanInfo {
			/// {true} if the current function is a constructor.
			bool init = false;

			/// {true} if the current value is being assigned to.
			bool assign = false;

			/// {true} if functions should be flagged as being used.
			bool useFunction = true;

			/// The type that is being initialized.
			Symbols::TypeSymbol* type = nullptr;

			/// File info.
			FileInfo file;

			/// Scope info.
			ScopeInfo scopeInfo;
		};
	}
}
