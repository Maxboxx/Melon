#include "FunctionSymbol.h"

#include "VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

FunctionSymbol::FunctionSymbol(const FileInfo& file) : ScopeSymbol(file) {
	
}

FunctionSymbol::~FunctionSymbol() {
	
}

Symbol* FunctionSymbol::ReturnType(const UInt index) const {
	return nullptr;
}

Symbol* FunctionSymbol::Argument(const UInt index) const {
	return nullptr;
}

Symbol* FunctionSymbol::TemplateArgument(const UInt index) const {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::AddOverload(FunctionSymbol* const overload) {
	overload->parent = this;
	overloads.Add(overload);
	return overload;
}

Symbol* FunctionSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	const Scope& scope = scopeList[index];

	if (scope.name.Size() == 0 && scope.arguments) {
		for (FunctionSymbol* const overload : overloads) {
			if (overload->Name() == scope) {
				return overload->Find(scopeList, index + 1, file);
			}
		}
	}
	else {
		return MapSymbol::Find(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}

Scope FunctionSymbol::Name() const {
	if (!overloads.IsEmpty()) {
		return name;
	}
	else {
		Scope name = this->name;

		if (!templateArguments.IsEmpty()) {
			name.types = templateArguments;
		}
		
		if (symbolNode) {
			name.arguments = arguments;
			return name;
		}
		else {
			List<ScopeList> args;

			for (UInt i = 0; i < arguments.Size(); i++) {
				if (Symbol* const type = Argument(i)) {
					args.Add(type->AbsoluteName());
				}
				else {
					args.Add(ScopeList::undefined);
				}
			}

			name.arguments = args;
			return name;
		}
	}
}
