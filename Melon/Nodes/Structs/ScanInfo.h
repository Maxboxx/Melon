#pragma once

#include "ScopeInfo.h"

#include "Melon/Errors.h";

namespace Melon {
	namespace Symbols {
		class TypeSymbol;
	}

	namespace Nodes {
		///B ScanInfo
		/// Used for info when scanning
		struct ScanInfo {
			///T Init
			/// true if the current function is an initializer
			bool init = false;

			///T Assign
			/// true if the current value is being assigned to
			bool assign = false;

			///T Use Function
			/// true if functions should count as being used 
			bool useFunction = true;

			///T Type
			/// The type that is being initialized
			Symbols::TypeSymbol* type = nullptr;

			///T File Info
			FileInfo file;

			///T Scope Info
			ScopeInfo scopeInfo;
		};
	}
}
