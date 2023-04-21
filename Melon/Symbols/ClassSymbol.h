#pragma once

#include "ClassStructBaseSymbol.h"

namespace Melon {
	namespace Nodes {
		class Node;
		class ClassStatement;
	}

	///N Melon::Symbols
	namespace Symbols {
		/// Contains information about a class.
		class ClassSymbol : public ClassStructBaseSymbol {
		public:
			ClassSymbol(const FileInfo& file);
			~ClassSymbol();
			
			virtual ClassSymbol* SpecializeTemplate(const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

			virtual Boxx::UInt Size() const override;

			virtual Kiwi::Type KiwiType() override;

			/// The class statement for this symbol.
			Weak<Melon::Nodes::ClassStatement> node;
		};
	}
}
