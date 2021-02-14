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
			bool init = false;
			bool assign = false;
			bool useFunction = true;

			Symbols::TypeSymbol* type = nullptr;

			FileInfo file;

			ScopeInfo scopeInfo;
		};
	}
}
