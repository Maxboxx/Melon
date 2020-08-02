#pragma once

#include "SymbolNode.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B IntegerSymbolNode
			/// Base for integer symbols
			class IntegerSymbolNode : public SymbolNode {
			public:

				///T Size
				/// The byte size for the integer
				Boxx::UByte size;
			};
		}
	}
}