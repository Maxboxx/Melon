#pragma once

#include "ClassStructBaseSymbol.h"

namespace Melon {
	namespace Nodes {
		class Node;
	}

	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a struct.
		class StructSymbol : public ClassStructBaseSymbol {
		public:
			StructSymbol(const FileInfo& file);
			~StructSymbol();
			
			virtual StructSymbol* InitializeSpecialize() override;

			/// {true} if the scope is recursive.
			bool isRecursive = false;

			/// The struct node for this symbol.
			Weak<Melon::Nodes::StructStatement> node;
		};
	}
}
