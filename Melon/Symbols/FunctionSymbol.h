#pragma once

#include "MapSymbol.h"

namespace Melon {
	namespace Symbols {
		class FunctionSymbol : public MapSymbol {
		public:
			FunctionSymbol(const FileInfo& file);
			~FunctionSymbol();

			///T Return Type
			/// Get the return type at the specified index
			Symbol* ReturnType(const Boxx::UInt index) const;

			///T Argument
			/// Get the argument at the specified index
			Symbol* Argument(const Boxx::UInt index) const;

			///T Template Argument
			/// Get the template argument at the specified index
			Symbol* TemplateArgument(const Boxx::UInt index) const;

			Boxx::List<ScopeList> templateArguments;
			Boxx::List<ScopeList> returnValues;
			Boxx::List<Scope> arguments;

		protected:
			virtual Symbol* Find(const ScopeList& scopeList, const Boxx::UInt index, const FileInfo& file) override;

		private:
			Boxx::List<FunctionSymbol*> overloads;
		};
	}
}
