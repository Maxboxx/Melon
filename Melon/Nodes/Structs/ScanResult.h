#pragma once

#include "Boxx/Types.h"

#include "ScanInfoStack.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Nodes {
		///B ScanResult
		/// The result of a node scan
		struct ScanResult {
			///T Self Used
			bool selfUsed = false;

			///T Self Use Check
			/// Check if self was used before being initialized and adds an error to the error log if it was.
			void SelfUseCheck(const ScanInfoStack& info, const FileInfo& file);

			///T Combine ScanResults
			ScanResult operator|(const ScanResult& result) const {
				ScanResult r;
				r.selfUsed = selfUsed || result.selfUsed;
				return r;
			}

			///T Combine ScanResults
			void operator|=(const ScanResult& result) {
				selfUsed = selfUsed || result.selfUsed;
			}
		};
	}
}
