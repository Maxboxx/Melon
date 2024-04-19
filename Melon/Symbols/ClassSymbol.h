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
			/// The base class.
			Boxx::Optional<NameList> baseClass;

			/// True if the class is abstract.
			bool abstract = false;

			ClassSymbol(const FileInfo& file);
			~ClassSymbol();
			
			virtual ClassSymbol* InitializeSpecialize() override;

			virtual Boxx::UInt Size() const override;

			virtual Kiwi::Type KiwiType() override;

			/// The base class.
			ClassSymbol* BaseClass() const;

			/// The class statement for this symbol.
			Weak<Melon::Nodes::ClassStatement> node;

		protected:
			virtual Symbol* FindSymbol(const NameList& nameList, const Boxx::UInt index, const FileInfo& file);
		};
	}
}
