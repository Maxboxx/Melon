#pragma once

#include "Boxx/Pointer.h"

#include "Symbol.h"
#include "ScopeSymbol.h"

namespace Melon {
	namespace Symbols {
		class SymbolTable {
		public:
			static Symbol* Find(const ScopeList& name, const FileInfo& file);
			static Symbol* Contains(const ScopeList& name);

		private:
			static Boxx::Pointer<ScopeSymbol> symbols;
		};
	}
}
