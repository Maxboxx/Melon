#pragma once

#include "Boxx/Types.h"

#include "ScanInfoStack.h"

#include "Melon/Errors.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// The result of a node scan.
		struct ScanResult {
			/// Has self been used.
			bool selfUsed = false;

			/// Check if self was used before being initialized and adds an error to the error log if it was.
			void SelfUseCheck(const ScanInfoStack& info, const FileInfo& file);

			/// Combines scan results.
			ScanResult operator|(const ScanResult& result) const {
				ScanResult r;
				r.selfUsed = selfUsed || result.selfUsed;
				return r;
			}

			/// Combines scan results.
			void operator|=(const ScanResult& result) {
				selfUsed = selfUsed || result.selfUsed;
			}
		};
	}
}
