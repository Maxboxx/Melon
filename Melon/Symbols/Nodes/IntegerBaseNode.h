#pragma once

#include "SymbolNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Base for integer symbols.
			template <class T>
			class IntegerBaseNode : public T {
			public:
				/// The byte size for the integer.
				Boxx::UByte size;
			};
		}
	}
}

