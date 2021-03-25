#pragma once

#include "StackPtr.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains info about the compiliation.
		struct CompileInfo {
			/// The next free label index.
			Boxx::UInt label = 0;

			/// The current stack and frame positions.
			StackPtr stack;

			/// The next free register index.
			Boxx::UByte index = 0;

			/// {true} if assignments to memory values are important.
			bool important = false;
		};
	}
}
