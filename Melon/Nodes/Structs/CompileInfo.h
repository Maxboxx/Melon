#pragma once

#include "StackPtr.h"

namespace Melon {
	namespace Nodes {
		///B CompileInfo
		/// Contains info about the compiliation
		struct CompileInfo {
			///T Next free label index
			Boxx::UInt label = 0;

			///T Stack
			StackPtr stack;

			///T Next free register index
			Boxx::UByte index = 0;

			///T Important stack values
			bool important = false;
		};
	}
}
